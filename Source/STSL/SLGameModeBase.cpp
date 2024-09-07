// Fill out your copyright notice in the Description page of Project Settings.

#include "SLGameModeBase.h"
#include "SellArea.h"
#include <Kismet/GameplayStatics.h>
#include "MainMenuBase.h"

void ASLGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SellAreaActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASellArea::StaticClass());
	if (ASellArea* SellArea = Cast<ASellArea>(SellAreaActor))
	{
		SellArea->OnSellCard.AddDynamic(this, &ASLGameModeBase::OnSellCardHandler);
	}
}

void ASLGameModeBase::Tick(float DeltaTime)
{
	if (CurrentPlayState == GamePlayState::BreakState) return;

	if (CurrentPlayState == GamePlayState::PlayState)
	{
		Time += DeltaTime;
	}
	if (Time >= TimeForDay)
	{
		BreakGame();
	}
}

void ASLGameModeBase::CreateMenu()
{
	MainMenu = CreateWidget(GetWorld(), LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/MainMenu.MainMenu_C'")));
	MainMenu->AddToViewport();
}

ASLGameModeBase::ASLGameModeBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/CardDB.CardDB'"));
	if (DataTable.Succeeded())
	{
		CardDataTable = DataTable.Object;
	}

	WorldBorder.Add(TEXT("Left"), -3000.0f);
	WorldBorder.Add(TEXT("Right"), 3000.0f);
	WorldBorder.Add(TEXT("Down"), -2000.0f);
	WorldBorder.Add(TEXT("Up"), 2000.0f);

}

void ASLGameModeBase::BreakGame()
{
	if (CurrentPlayState == GamePlayState::PlayState)
	{
		Time = 0;
		CurrentPlayState = GamePlayState::BreakState;
		BreakMenu = CreateWidget(GetWorld(), LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/BreakMenu.BreakMenu_C'")));
		BreakMenu->AddToViewport();
		BreakMenu->GetWidgetFromName(TEXT("EatButton"))->SetVisibility(ESlateVisibility::Visible);
		BreakMenu->GetWidgetFromName(TEXT("CardIndicator"))->SetVisibility(ESlateVisibility::Hidden);
		BreakMenu->GetWidgetFromName(TEXT("NextDayIndicator"))->SetVisibility(ESlateVisibility::Hidden);
		for (TObjectPtr<ACardStack> CardStack : CardStacks)
		{
			CardStack->BreakGame();
		}
	}
	if (CurrentPlayState == GamePlayState::PauseState && bSellingExcessiveCard)
	{
		bSellingExcessiveCard = false;
		CurrentPlayState = GamePlayState::BreakState;
		for (TObjectPtr<ACardStack> CardStack : CardStacks)
		{
			CardStack->BreakGame();
		}
	}
	Cast<UMainMenuBase>(MainMenu)->UpdateIcon(CurrentPlayState);
}

void ASLGameModeBase::Eat()
{
	if (CurrentPlayState == GamePlayState::BreakState)
	{
		BreakMenu->GetWidgetFromName(TEXT("EatButton"))->SetVisibility(ESlateVisibility::Hidden);
		People.Empty();
		for (TObjectPtr<ACardStack> CardStack : CardStacks)
		{
			for (TObjectPtr<ACard> Person : CardStack->GetCardsByType(CardType::person))
			{
				Cast<APersonCard>(Person)->ResetFood();
				People.Add(Person);
			}
		}
		int RequireFood = People.Num() * 2;

		Foods.Empty();
		for (TObjectPtr<ACardStack> CardStack : CardStacks)
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

		// TODO: ���ĵ��� �켱�������� �����ϰų� �켱������ �ľ�
		PersonIndex = 0; FoodIndex = 0;

		EatNext();
	}
}

void ASLGameModeBase::EatNext()
{
	if (FoodIndex == Foods.Num())
	{
		FoodIndex = 0;
		PersonIndex++;
		if (PersonIndex == People.Num())
		{
			CheckExcessiveCards();
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
	}
	else {
		FCardAnimationCallback Callback;
		Callback.BindUObject(this, &ASLGameModeBase::MoveBackCompleted);
		CurrentFood->MoveBack(Callback);
		CurrentFood->LoadCard();
	}
	FoodIndex++;
	EatNext();
}

void ASLGameModeBase::MoveBackCompleted()
{
	FoodIndex++;
	EatNext();
}

void ASLGameModeBase::OnSellCardHandler()
{
	if (bSellingExcessiveCard) CheckExcessiveCards();
}

void ASLGameModeBase::CheckExcessiveCards()
{
	if (GetExcessiveCardAmount() < 1)
	{
		CheckDayEnd();
		return;
	}
	BreakMenu->GetWidgetFromName(TEXT("CardIndicator"))->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	bSellingExcessiveCard = true;
	PauseGame();
}

void ASLGameModeBase::CheckDayEnd()
{
	BreakMenu->GetWidgetFromName(TEXT("CardIndicator"))->SetVisibility(ESlateVisibility::Hidden);
	BreakMenu->GetWidgetFromName(TEXT("NextDayIndicator"))->SetVisibility(ESlateVisibility::Visible);
	BreakGame();
}

void ASLGameModeBase::EndDay()
{
	Day++;
	BreakMenu->RemoveFromParent();
	if ((Day - PortalStartingDay) % PortalIntervalDay == 0)
	{
		FVector Location(500.0f, 0.0f, 0.0f);
		SpawnCard(Location, 33);
	}
	ResumeGame();
}

int ASLGameModeBase::GetPortalSpawnCount() const
{
	int SpawnCount = 0;
	SpawnCount = (Day - PortalStartingDay) / PortalIntervalDay + InitSpawnCount;
	return FMath::Min(SpawnCount, MaxSpawnCount);
}

void ASLGameModeBase::PauseGame()
{
	if (CurrentPlayState == GamePlayState::PlayState)
	{
		CurrentPlayState = GamePlayState::PauseState;
	}
	if (CurrentPlayState == GamePlayState::BreakState)
	{
		CurrentPlayState = GamePlayState::PauseState;
		for (TObjectPtr<ACardStack> CardStack : CardStacks)
		{
			CardStack->ResumeGame();
		}
	}
	Cast<UMainMenuBase>(MainMenu)->UpdateIcon(CurrentPlayState);
}

void ASLGameModeBase::ResumeGame()
{
	if (CurrentPlayState == GamePlayState::PauseState)
	{
		if (bSellingExcessiveCard) return;
		CurrentPlayState = GamePlayState::PlayState;
	}
	if (CurrentPlayState == GamePlayState::BreakState)
	{
		CurrentPlayState = GamePlayState::PlayState;
		for (TObjectPtr<ACardStack> CardStack : CardStacks)
		{
			CardStack->ResumeGame();
		}
	}
	Cast<UMainMenuBase>(MainMenu)->UpdateIcon(CurrentPlayState);
}

float ASLGameModeBase::GetDayProgressPercent() const
{
	return FMath::Clamp(Time / TimeForDay, 0.0f, 1.0f);
}

void ASLGameModeBase::StartBattle(ACardStack* FirstStack, ACardStack* SecondStack) const
{
	TArray<ACharacterCard*> FirstTeam;
	TArray<ACharacterCard*> SecondTeam;

	for (AActor* FirstActor : FirstStack->GetAllCharacters())
	{
		FirstTeam.Add(Cast<ACharacterCard>(FirstActor));
	}

	for (AActor* SecondActor : SecondStack->GetAllCharacters())
	{
		SecondTeam.Add(Cast<ACharacterCard>(SecondActor));
	}

	FVector Location = FVector::Zero();

	AActor* NewBattleManager = GetWorld()->SpawnActor
	(
		ABattleManager::StaticClass(),
		&Location
	);

	Cast<ABattleManager>(NewBattleManager)->SetTeam(FirstTeam, SecondTeam);
}

int ASLGameModeBase::GetTotalCardAmount(bool ExcludeCoin = false) const
{
	int Sum = 0;
	for (ACardStack* CardStack : CardStacks)
	{
		Sum += CardStack->GetCardAmount(ExcludeCoin);
	}
	return Sum;
}

void ASLGameModeBase::AddCardStack(ACardStack* CardStack)
{
	CardStacks.Add(CardStack);
}

void ASLGameModeBase::RemoveCardStack(ACardStack* CardStack)
{
	if (CardStack == DraggingStack)
	{
		DraggingStack = nullptr;
	}
	CardStacks.Remove(CardStack);
}


TMap<FString, float> ASLGameModeBase::GetWorldBorder(bool bExcludeBuyArea) const
{
	if (bExcludeBuyArea)
	{
		TMap<FString, float> Border = WorldBorder;
		Border["Up"] -= BuyAreaHeight;
		return Border;
	}
	else {
		return WorldBorder;
	}
}

void ASLGameModeBase::SetCardHighlight(bool bCardHighlight, ACardStack* NewDraggingStack)
{
	bIsCardHighlight = bCardHighlight;
	if (bIsCardHighlight)
	{
		DraggingStack = NewDraggingStack;
		if (DraggingStack == nullptr) return;
		for (ACardStack* CardStack : CardStacks)
		{
			if (CardStack != DraggingStack && ACardStack::GetCardStackable(DraggingStack, CardStack))
			{
				CardStack->GetLastCard()->GetVisualMesh()->SetRenderCustomDepth(true);
			}
		}
	} 
	else {
		for (ACardStack* CardStack : CardStacks)
		{
			CardStack->GetLastCard()->GetVisualMesh()->SetRenderCustomDepth(false);
		}
	}
}

ACardStack* ASLGameModeBase::SpawnCard(FVector Location, int CardID)
{
	DraggingStack = nullptr;

	AActor* NewCardStackActor = GetWorld()->SpawnActor
	(
		ACardStack::StaticClass(),
		&Location
	);

	if (NewCardStackActor == nullptr) return nullptr;
	ACardStack* NewCardStack = Cast<ACardStack>(NewCardStackActor);
	 
	FName RowName = FName(*FString::FromInt(CardID));
	FCardData* RowData = CardDataTable->FindRow<FCardData>(RowName, TEXT(""));
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

	return NewCardStack;
}

ACardStack* ASLGameModeBase::SpawnCardPack(FVector Location, int PackID)
{
	DraggingStack = nullptr;

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
