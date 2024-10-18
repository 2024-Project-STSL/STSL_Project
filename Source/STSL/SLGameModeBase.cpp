// Fill out your copyright notice in the Description page of Project Settings.

#include "SLGameModeBase.h"
#include "SellArea.h"
#include <Kismet/GameplayStatics.h>
#include <Engine/AssetManager.h>
#include <SLGameInstance.h>
#include "MainMenuBase.h"
#include <BuyArea.h>

void ASLGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(AutoSaveHandle, this, &ASLGameModeBase::SaveGame, AutoSaveInterval, true);

	SellAreaActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASellArea::StaticClass());
	if (ASellArea* SellArea = Cast<ASellArea>(SellAreaActor))
	{
		SellArea->OnSellCard.AddDynamic(this, &ASLGameModeBase::OnSellCardHandler);
	}

	FString BattleManagerPath = "/Script/CoreUObject.Class'/Script/STSL.BattleManager'";
	UAssetManager::GetStreamableManager().RequestAsyncLoad(BattleManagerPath);
}

void ASLGameModeBase::StartPlay()
{
	FieldManager = Cast<AFieldManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AFieldManager::StaticClass())
	);

	SLGameState = GetGameState<ASLGameStateBase>();

	Super::StartPlay();
}

void ASLGameModeBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(AutoSaveHandle);
}

void ASLGameModeBase::Tick(float DeltaTime) {

	if (SLGameState->CurrentPlayState == GamePlayState::BreakState) return;

	if (SLGameState->CurrentPlayState == GamePlayState::PlayState)
	{
		SLGameState->Time += DeltaTime;
	}
	if (SLGameState->Time >= SLGameState->TimeForDay)
	{
		SaveGame();
		BreakGame();
	}
}

void ASLGameModeBase::CreateMenu()
{
	if (MainMenu == nullptr)
	{
		MainMenu = CreateWidget(GetWorld(), LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/MainMenu.MainMenu_C'")));
	}
	MainMenu->AddToViewport();
}

ASLGameModeBase::ASLGameModeBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameStateClass = ASLGameStateBase::StaticClass();

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/CardDB.CardDB'"));
	if (DataTable.Succeeded())
	{
		CardDataTable = DataTable.Object;
	}

}

void ASLGameModeBase::BreakGame()
{

	if (SLGameState->CurrentPlayState == GamePlayState::PlayState)
	{
		SLGameState->Time = 0;
		SLGameState->CurrentPlayState = GamePlayState::BreakState;
		BreakMenu = CreateWidget(GetWorld(), LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/BreakMenu.BreakMenu_C'")));
		BreakMenu->AddToViewport();
		BreakMenu->GetWidgetFromName(TEXT("EatButton"))->SetVisibility(ESlateVisibility::Visible);
		BreakMenu->GetWidgetFromName(TEXT("CardIndicator"))->SetVisibility(ESlateVisibility::Hidden);
		BreakMenu->GetWidgetFromName(TEXT("NextDayIndicator"))->SetVisibility(ESlateVisibility::Hidden);
		for (TObjectPtr<ACardStack> CardStack : SLGameState->CardStacks)
		{
			CardStack->BreakGame();
		}
	}
	if (SLGameState->CurrentPlayState == GamePlayState::PauseState && SLGameState->bSellingExcessiveCard)
	{
		SLGameState->bSellingExcessiveCard = false;
		SLGameState->CurrentPlayState = GamePlayState::BreakState;
		for (TObjectPtr<ACardStack> CardStack : SLGameState->CardStacks)
		{
			CardStack->BreakGame();
		}
	}
	Cast<UMainMenuBase>(MainMenu)->UpdateIcon(SLGameState->CurrentPlayState);
}

void ASLGameModeBase::Eat()
{

	if (SLGameState->CurrentPlayState == GamePlayState::BreakState)
	{
		BreakMenu->GetWidgetFromName(TEXT("EatButton"))->SetVisibility(ESlateVisibility::Hidden);
		People.Empty();
		for (TObjectPtr<ACardStack> CardStack : SLGameState->CardStacks)
		{
			for (TObjectPtr<ACard> Person : CardStack->GetCardsByType(CardType::person))
			{
				Cast<APersonCard>(Person)->ResetFood();
				People.Add(Person);
			}
		}
		int RequireFood = People.Num() * 2;

		Foods.Empty();
		for (TObjectPtr<ACardStack> CardStack : SLGameState->CardStacks)
		{
			for (TObjectPtr<ACard> Food : CardStack->GetCardsByType(CardType::food))
			{
				Foods.Add(Food);
			}
		}

		int TotalFood = 0;
		for (TObjectPtr<ACard> Food : Foods)
		{
			TotalFood += Food->GetAddTypeValue();
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%d / %d food points needed."), TotalFood, RequireFood));

		// TODO: 음식들을 우선순위별로 정렬하거나 우선순위를 파악
		PersonIndex = 0; FoodIndex = -1;

		EatNext();
	}
}

void ASLGameModeBase::EatNext()
{
	FoodIndex++;
	if (FoodIndex >= Foods.Num())
	{
		FoodIndex = 0;
		PersonIndex++;
		if (PersonIndex == People.Num())
		{
			CheckHunger();
			return;
		}
	}

	TObjectPtr<APersonCard> CurrentPerson = Cast<APersonCard>(People[PersonIndex]);
	TObjectPtr<ACard> CurrentFood = Foods[FoodIndex];
	FCardAnimationCallback Callback;
	Callback.BindUObject(this, &ASLGameModeBase::EatCompleted);
	CurrentPerson->Eat(CurrentFood, Callback);

}

void ASLGameModeBase::EatCompleted()
{
	TObjectPtr<ACard> CurrentFood = Foods[FoodIndex];
	if (CurrentFood->GetAddTypeValue() <= 0)
	{
		TObjectPtr<ACardStack> FoodStack = Cast<ACardStack>(CurrentFood->GetCardStack());
		FoodStack->RemoveCard(CurrentFood, true);
		EatNext();
	}
	else {
		FCardAnimationCallback Callback;
		Callback.BindUObject(this, &ASLGameModeBase::MoveBackCompleted);
		CurrentFood->MoveBack(Callback);
		CurrentFood->LoadCard();
	}
}

void ASLGameModeBase::MoveBackCompleted()
{
	EatNext();
}

void ASLGameModeBase::CheckHunger()
{
	for (ACard* PersonCard : People)
	{
		TObjectPtr<APersonCard> Person = Cast<APersonCard>(PersonCard);
		if (!Person->IsFull())
		{
			Person->CharacterDeath(EDeathReason::Hunger);
		}
	}
	if (!CheckGameover())
	{
		CheckExcessiveCards();
	}
}

void ASLGameModeBase::OnSellCardHandler()
{
	UpdateCardLimit();
	if (SLGameState->bSellingExcessiveCard) CheckExcessiveCards();
}

bool ASLGameModeBase::CheckGameover()
{
	for (TObjectPtr<ACardStack> CardStack : SLGameState->CardStacks)
	{
		if (CardStack->IsPendingKillPending()) continue;

		for (ACard* Person : CardStack->GetCardsByType(CardType::person))
		{
			if (Cast<ACharacterCard>(Person)->IsAilve()) return false;
		}
	}
	Gameover();
	return true;
}

void ASLGameModeBase::CheckExcessiveCards()
{
	if (GetExcessiveCardAmount() < 1)
	{
		CheckDayEnd();
		return;
	}
	BreakMenu->GetWidgetFromName(TEXT("CardIndicator"))->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SLGameState->bSellingExcessiveCard = true;
	PauseGame(true);
}

void ASLGameModeBase::CheckDayEnd()
{
	BreakMenu->GetWidgetFromName(TEXT("CardIndicator"))->SetVisibility(ESlateVisibility::Hidden);
	BreakMenu->GetWidgetFromName(TEXT("NextDayIndicator"))->SetVisibility(ESlateVisibility::Visible);
	BreakGame();
}

void ASLGameModeBase::EndDay()
{
	SLGameState->Day++;
	BreakMenu->RemoveFromParent();
	if ((SLGameState->Day - SLGameState->PortalStartingDay) % SLGameState->PortalIntervalDay == 0)
	{
		FVector Location(500.0f, 0.0f, 0.0f);
		SpawnCard(Location, 33);
	}
	ResumeGame();
	SaveGame();
}

int ASLGameModeBase::GetPortalSpawnCount() const
{
	int SpawnCount = 0;
	SpawnCount = (SLGameState->Day - SLGameState->PortalStartingDay) / SLGameState->PortalIntervalDay + SLGameState->InitSpawnCount;
	return FMath::Min(SpawnCount, SLGameState->MaxSpawnCount);
}

void ASLGameModeBase::PauseGame(bool bForce)
{
	if (SLGameState->CurrentPlayState == GamePlayState::PlayState)
	{
		SLGameState->CurrentPlayState = GamePlayState::PauseState;
		if (PauseMenu == nullptr)
		{
			PauseMenu = CreateWidget(GetWorld(), LoadClass<UUserWidget>(
				nullptr,
				TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/PauseMenu.PauseMenu_C'"))
			);
		}
		PauseMenu->AddToViewport();
	}
	if (SLGameState->CurrentPlayState == GamePlayState::BreakState && bForce)
	{
		SLGameState->CurrentPlayState = GamePlayState::PauseState;
		for (TObjectPtr<ACardStack> CardStack : SLGameState->CardStacks)
		{
			CardStack->ResumeGame();
		}
	}
	Cast<UMainMenuBase>(MainMenu)->UpdateIcon(SLGameState->CurrentPlayState);
}

void ASLGameModeBase::ResumeGame()
{
	if (SLGameState->CurrentPlayState == GamePlayState::PauseState)
	{
		if (SLGameState->bSellingExcessiveCard) return;

		if (PauseMenu != nullptr && PauseMenu->IsInViewport())
		{
			PauseMenu->RemoveFromParent();
		}

		SLGameState->CurrentPlayState = GamePlayState::PlayState;
	}
	if (SLGameState->CurrentPlayState == GamePlayState::BreakState)
	{
		SLGameState->CurrentPlayState = GamePlayState::PlayState;
		for (TObjectPtr<ACardStack> CardStack : SLGameState->CardStacks)
		{
			CardStack->ResumeGame();
		}
	}
	Cast<UMainMenuBase>(MainMenu)->UpdateIcon(SLGameState->CurrentPlayState);
}

void ASLGameModeBase::Gameover()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Game Over!"));
	if (PauseMenu != nullptr && PauseMenu->IsInViewport())
	{
		PauseMenu->RemoveFromParent();
	}
	if (GameoverMenu == nullptr)
	{
		GameoverMenu = CreateWidget(GetWorld(), LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/GameoverMenu.GameoverMenu_C'")));
	}
	SLGameState->CurrentPlayState = GamePlayState::GameoverState;
	GameoverMenu->AddToViewport();
	GEngine->GetFirstLocalPlayerController(GetWorld())->SetPause(true);
}

float ASLGameModeBase::GetDayProgressPercent() const
{
	return FMath::Clamp(SLGameState->Time / SLGameState->TimeForDay, 0.0f, 1.0f);
}

void ASLGameModeBase::SaveGame() const
{
	if (SLGameState->CurrentPlayState == GamePlayState::BreakState) return;
	if (SLGameState->bSellingExcessiveCard) return;

	USLSaveGame* SaveGame = SLGameState->GetSaveGame();

	TArray<AActor*> BuyAreaActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuyArea::StaticClass(), BuyAreaActors);

	for (AActor* BuyAreaActor : BuyAreaActors)
	{
		SaveGame->CurrentBuyAreaPrice.Add(BuyAreaActor->GetName(), Cast<ABuyArea>(BuyAreaActor)->GetCurrentCardPrice());
	}

	if (!UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("STSLSave"), 0))
	{
		UE_LOG(LogClass, Warning, TEXT("Error occurred at game saving"));
	}
}

void ASLGameModeBase::ResetGame()
{
	USLGameInstance* GameInstance = Cast<USLGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (GameInstance->IsLoadGame())
	{
		USLSaveGame* SaveGame = Cast<USLSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("STSLSave"), 0));

		if (SaveGame == nullptr)
		{
			UE_LOG(LogClass, Warning, TEXT("Error occurred at game loading"));
			return;
		}

		// GameState 불러오기
		SLGameState->Day = SaveGame->Day;
		SLGameState->Time = SaveGame->Time;
		SLGameState->CardLimit = SaveGame->CardLimit;
		FieldManager->SetWorldBorder(SLGameState->CardLimit, SLGameState->BaseCardLimit);

		// CurrentPlayState는 실제 게임에 적용하기 위해 아래에서 처리

		TArray<AActor*> BuyAreaActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuyArea::StaticClass(), BuyAreaActors);

		for (AActor* BuyAreaActor : BuyAreaActors)
		{
			Cast<ABuyArea>(BuyAreaActor)->SetCurrentCardPrice(SaveGame->CurrentBuyAreaPrice[BuyAreaActor->GetName()]);
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%d Stacks loaded."), SaveGame->AllCardStacks.Num()));

		for (int i = 0; i < SaveGame->AllCardStacks.Num(); i++)
		{
			auto& CardStackData = SaveGame->AllCardStacks[i];
			if (CardStackData.IsEmpty()) continue;

			ACardStack* NewCardStack = SpawnCard(CardStackData.CardLocation, CardStackData.GetNextCardCode());
			if (NewCardStack == nullptr) continue;

			CardStackData.LoadCard(NewCardStack->GetFirstCard());

			while (!CardStackData.IsEmpty())
			{
				ACardStack* NewCard = SpawnCard(CardStackData.CardLocation, CardStackData.GetNextCardCode());
				CardStackData.LoadCard(NewCard->GetFirstCard());
				NewCardStack->AddCard(NewCard->GetFirstCard());
			}

			NewCardStack->SetCraftingData(CardStackData.CraftingProgress);
		}

		if (SaveGame->CurrentPlayState == GamePlayState::PauseState) PauseGame();
	}
	else {
		FVector Location = FVector(0.0f, -500.0f, 0.0f);
		SpawnCard(Location, 4); // 사람

		Location = FVector(0.0f, 500.0f, 0.0f);
		SpawnCardPack(Location, 1); // 일반 카드팩

		SaveGame();
	}

}

void ASLGameModeBase::HandleDeath(ACharacterCard* TargetCard)
{
	CheckGameover();
}

void ASLGameModeBase::StartBattle(ACardStack* FirstStack, ACardStack* SecondStack) const
{
	TArray<ACharacterCard*> FirstTeam;
	TArray<ACharacterCard*> SecondTeam;
	TMap<AActor*, int> AllActors;

	for (AActor* FirstActor : FirstStack->GetAllCharacters())
	{
		FirstTeam.Add(Cast<ACharacterCard>(FirstActor));
		AllActors.Add(FirstActor, 1);
	}

	for (AActor* SecondActor : SecondStack->GetAllCharacters())
	{
		SecondTeam.Add(Cast<ACharacterCard>(SecondActor));
		AllActors.Add(SecondActor, 2);
	}

	
	for (TTuple<AActor*, int> Character : AllActors)
	{
		if (Character.Value == 1)
		{
			FirstStack->RemoveCard(Character.Key, false);
		}
		else {
			SecondStack->RemoveCard(Character.Key, false);
		}

		FVector Location = Character.Key->GetActorLocation();

		AActor* NewCardStackActor = Character.Key->GetWorld()->SpawnActor
		(
			ACardStack::StaticClass(),
			&Location
		);

		if (NewCardStackActor == nullptr) return;
		ACardStack* NewCardStack = Cast<ACardStack>(NewCardStackActor);

		NewCardStack->AddCard(Character.Key);
	}

	FVector Center = FVector::Zero();
	FVector BoxExtent;

	TArray<AActor*> AllChars;
	AllActors.GetKeys(AllChars);

	UGameplayStatics::GetActorArrayBounds(AllChars, false, Center, BoxExtent);
	Center.Z = 0.0f;

	AActor* NewBattleManager = GetWorld()->SpawnActor
	(
		ABattleManager::StaticClass(),
		&Center
	);

	Cast<ABattleManager>(NewBattleManager)->SetTeam(FirstTeam, SecondTeam);
}

int ASLGameModeBase::GetTotalCardAmount(bool ExcludeCoin = false) const
{
	int Sum = 0;
	for (ACardStack* CardStack : SLGameState->CardStacks)
	{
		Sum += CardStack->GetCardAmount(ExcludeCoin);
	}
	return Sum;
}

void ASLGameModeBase::AddCardStack(ACardStack* CardStack)
{
	SLGameState->CardStacks.Add(CardStack);
}

void ASLGameModeBase::RemoveCardStack(ACardStack* CardStack)
{
	if (CardStack == SLGameState->DraggingStack)
	{
		SLGameState->DraggingStack = nullptr;
	}
	SLGameState->CardStacks.Remove(CardStack);
}


void ASLGameModeBase::UpdateCardLimit()
{
	SLGameState->CardLimit = SLGameState->BaseCardLimit;
	TArray<TObjectPtr<ACard>> Extendments;
	
	for (TObjectPtr<ACardStack> CardStack : SLGameState->CardStacks)
	{
		Extendments = CardStack->GetCardsByType(CardType::extendment);
		for (TObjectPtr<ACard> Extendment : Extendments)
		{
			SLGameState->CardLimit += Extendment->GetAddTypeValue();
		}
	}

	FieldManager->SetWorldBorder(SLGameState->CardLimit, SLGameState->BaseCardLimit);
	
	for (TObjectPtr<ACardStack> CardStack : SLGameState->CardStacks)
	{
		CardStack->UpdateWorldBorder();
	}
}

void ASLGameModeBase::SetCardHighlight(bool bCardHighlight, ACardStack* NewDraggingStack)
{
	SLGameState->bIsCardHighlight = bCardHighlight;
	if (SLGameState->bIsCardHighlight)
	{
		SLGameState->DraggingStack = NewDraggingStack;
		if (SLGameState->DraggingStack == nullptr) return;
		for (ACardStack* CardStack : SLGameState->CardStacks)
		{
			if (CardStack != SLGameState->DraggingStack 
				&& (ACardStack::GetCardStackable(SLGameState->DraggingStack, CardStack) 
					|| ACardStack::GetCardBattleable(SLGameState->DraggingStack, CardStack)))
			{
				CardStack->GetLastCard()->GetVisualMesh()->SetRenderCustomDepth(true);
			}
		}
	} 
	else {
		for (ACardStack* CardStack : SLGameState->CardStacks)
		{
			CardStack->GetLastCard()->GetVisualMesh()->SetRenderCustomDepth(false);
		}
	}
}

ACardStack* ASLGameModeBase::SpawnCard(FVector Location, int CardID)
{
	if (CardID > 10000)
	{
		return SpawnCardPack(Location, CardID - 10000);
	}

	FName RowName = FName(*FString::FromInt(CardID));
	FCardData* RowData = CardDataTable->FindRow<FCardData>(RowName, TEXT(""));
	if (RowData == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("존재하지 않는 카드 번호: %d"), CardID));
		return nullptr;
	}

	SLGameState->DraggingStack = nullptr;

	AActor* NewCardStackActor = GetWorld()->SpawnActor
	(
		ACardStack::StaticClass(),
		&Location
	);

	if (NewCardStackActor == nullptr) return nullptr;
	ACardStack* NewCardStack = Cast<ACardStack>(NewCardStackActor);
	 
	UClass* CardClass = ACard::StaticClass();

	if (RowData != nullptr)
	{
		if (RowData->IsCharacter())
		{
			if (RowData->CardType == CardType::person)
			{
				CardClass = APersonCard::StaticClass();
			}
			else {
				CardClass = ACharacterCard::StaticClass();
			}
		}
		else if (RowData->CardType == CardType::portal)
		{
			CardClass = APortalCard::StaticClass();
		}
		else {
			CardClass = ACard::StaticClass();
		}
	}

	AActor* NewCardActor = GetWorld()->SpawnActor
	(
		CardClass,
		&Location
	);

	if (NewCardActor == nullptr) return nullptr;
	ACard* NewCard = Cast<ACard>(NewCardActor);

	NewCard->SetCardID(CardID);
	NewCardStack->AddCard(NewCardActor);

	if (RowData->CardType == CardType::person)
	{
		Cast<APersonCard>(NewCardActor)->OnDeath.AddDynamic(this, &ASLGameModeBase::HandleDeath);
	} 
	else if (RowData->CardType == CardType::extendment)
	{
		UpdateCardLimit();
	}

	return NewCardStack;
}

ACardStack* ASLGameModeBase::SpawnCardPack(FVector Location, int PackID)
{
	SLGameState->DraggingStack = nullptr;

	AActor* NewCardStackActor = GetWorld()->SpawnActor
	(
		ACardStack::StaticClass(),
		&Location
	);

	if (NewCardStackActor == nullptr) return nullptr;
	ACardStack* NewCardStack = Cast<ACardStack>(NewCardStackActor);

	AActor* NewCardActor = GetWorld()->SpawnActor
	(
		ACardPack::StaticClass(),
		&Location
	);

	if (NewCardActor == nullptr) return nullptr;
	ACardPack* NewCard = Cast<ACardPack>(NewCardActor);

	NewCard->SetCardID(PackID);
	NewCardStack->AddCard(NewCardActor);

	return NewCardStack;
}
