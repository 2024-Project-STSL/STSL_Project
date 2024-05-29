// Fill out your copyright notice in the Description page of Project Settings.


#include "CardStack.h"
#include "SLGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACardStack::ACardStack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	RootComponent = SceneComponent;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/RecipeDB.RecipeDB'"));
	if (DataTable.Succeeded())
	{
		CraftingRecipeTable = DataTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DropDataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DropDB.DropDB'"));
	if (DropDataTable.Succeeded())
	{
		DropTable = DropDataTable.Object;
	}
}

// Called when the game starts or when spawned
void ACardStack::BeginPlay()
{
	Super::BeginPlay();

	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
	SLGameMode->AddCardStack(this);
}

void ACardStack::RemoveFromGamemode()
{
	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (SLGameMode != nullptr)
	{
		SLGameMode->RemoveCardStack(this);
	}
}

bool ACardStack::CheckCraftingRecipe(FRecipeData* Recipe)
{
	if (Recipe != nullptr)
	{
		int ReqAmount = Recipe->ReqCardCode.Num();
		int Amount = 0;
		int ReqCardCount = 0; // ���տ� ���� ī���� �� ����
		for (int i = 0; i < ReqAmount; i++)
		{
			int ReqCardCode = Recipe->ReqCardCode[i];
			int ReqCardAmount = Recipe->ReqValue[i];

			if (!CardCount.Contains(ReqCardCode)) break;
			if (CardCount[ReqCardCode] != ReqCardAmount) break;

			Amount++;
			ReqCardCount += ReqCardAmount;
		}

		// ���չ� ���� �� ī�尡 ������ ���չ� ��ȿ
		return (ReqAmount == Amount && ReqCardCount == Cards.Num());
	}
	else {
		return false;
	}
}

bool ACardStack::CheckDropRecipe(FDropData* Recipe)
{
	if (Cards.Num() < 2) return false;

	if (GetLastCard()->GetCardType() != CardType::person) return false;

	ACard* ProductionCard = Cast<ACard>(Cards[Cards.Num() - 2]);

	if (ProductionCard->GetAddType() != AddType::dropable) return false;
	
	return (ProductionCard->GetCardID() == Recipe->CardCode);
}

void ACardStack::UpdateCraftingRecipe()
{
	bool bIsCrafting = false;
	bool bIsRecipeChanged = false;

	for (const auto& Row : CraftingRecipeTable->GetRowMap())
	{
		int RecipeID = FCString::Atoi(*Row.Key.ToString());

		const uint8* RowData = Row.Value;

		FRecipeData* RecipeData = reinterpret_cast<FRecipeData*>(const_cast<uint8*>(RowData));
		if (CheckCraftingRecipe(RecipeData))
		{
			bIsCrafting = true;
			if (RecipeID != CraftingRecipeID)
			{
				CraftingRecipeID = RecipeID;
				bIsRecipeChanged = true;
			}
			break;
		}
	}
	if (!bIsCrafting) {
		for (const auto& Row : DropTable->GetRowMap())
		{
			int RecipeID = FCString::Atoi(*Row.Key.ToString());

			const uint8* RowData = Row.Value;

			FDropData* RecipeData = reinterpret_cast<FDropData*>(const_cast<uint8*>(RowData));
			if (CheckDropRecipe(RecipeData))
			{
				bIsCrafting = true;
				if (RecipeID != CraftingRecipeID)
				{
					CraftingRecipeID = RecipeID;
					bIsRecipeChanged = true;
				}
				break;
			}
		}
	}

	SetShowProgressBar(bIsCrafting);
	if (!bIsCrafting)
	{
		CraftingRecipeID = -1;
	}

	if (bIsRecipeChanged)
	{
		// TODO : �� ī�� ���� ���� �ð� ����

		// ���� ���൵ �ʱ�ȭ
		CraftingProgress = 0.0f;

		GetFirstCard()->UpdateProgressBar(CraftingProgress, MakeTime);
	}
}

void ACardStack::CompleteCrafting()
{
	FName RowName = FName(*FString::FromInt(CraftingRecipeID));
	CraftingProgress = 0.0f;
	CraftingRecipeID = -1;

	FRecipeData* RecipeData = CraftingRecipeTable->FindRow<FRecipeData>(RowName, TEXT(""));
	if (RecipeData != nullptr)
	{
		// ���� ������ ī��� ���� ������ų� ���� �� ������Ƿ�,
		// ������ ��� ī�� ���� ��ȸ
		// ī�尡 ������� ī�� index�� ������ �ʰ� �ϱ� ���� �������� ��ȸ
		for (int i = Cards.Num() - 1; i >= 0; i--)
		{
			ACard* Card = Cast<ACard>(Cards[i]);
			int RecipeIndex = RecipeData->ReqCardCode.IndexOfByKey<int>(Card->GetCardID());
			if (RecipeData->bIsDeleted[RecipeIndex])
			{
				RemoveCard(i, true);
			}
		}

		// ī��� 'Ƣ��;�' �ϹǷ�, ������� ���� ���� ��� ���ð� ������ ������� ����
		// ���� �� ������ �����ؼ� ����� ����
		FVector Location = GetActorLocation();

		Location.Z += 10;

		ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
		ACardStack* NewCardStack = nullptr;
		NewCardStack = SLGameMode->SpawnCard(Location, RecipeData->CardCode);
		if (NewCardStack != nullptr)
		{
			Cast<ACard>(NewCardStack->Cards[0])->Push();
		}
	}
	UpdateCraftingRecipe();
}

void ACardStack::CompleteProducing()
{
	FName RowName = FName(*FString::FromInt(CraftingRecipeID));
	CraftingProgress = 0.0f;
	CraftingRecipeID = -1;

	FDropData* DropData = DropTable->FindRow<FDropData>(RowName, TEXT(""));
	if (DropData != nullptr)
	{
		// ī��� 'Ƣ��;�' �ϹǷ�, ������� ���� ���� ��� ���ð� ������ ������� ����
		// ���� �� ������ �����ؼ� ����� ����
		FVector Location = GetActorLocation();

		Location.Z += 10;

		ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
		ACardStack* NewCardStack = nullptr;
		int TotalWeight = 0;
		for (int CardWeight : DropData->DropWeight)
		{
			TotalWeight += CardWeight;
		}
		int RandomNumber = FMath::RandRange(1, TotalWeight);
		for (int i = 0; i < DropData->DropCardCode.Num(); i++)
		{
			RandomNumber -= DropData->DropWeight[i];
			if (RandomNumber < 1)
			{
				NewCardStack = SLGameMode->SpawnCard(Location, DropData->DropCardCode[i]);
				break;
			}
		}
		if (NewCardStack != nullptr)
		{
			ACardStack* NearestStackable = NewCardStack->GetNearestStackable(SearchDistance, this);
			if (NearestStackable != nullptr)
			{
				NearestStackable->AddCard(NewCardStack->Cards[0]);
			}
			else {
				NewCardStack->GetFirstCard()->Push();
			}
		}
		// ������ ī�� = �ڿ��� 2��° ī��
		ACard* Card = Cast<ACard>(Cards[Cards.Num()-2]);
		if (Card->GetAddType() == AddType::dropable)
		{
			Card->SetAddTypeValue(Card->GetAddTypeValue() - 1);
		}
	}
	UpdateCraftingRecipe();
}

void ACardStack::UpdatePosition(bool bFalling)
{
	if (Cards.Num() == 0) return;

	// �ڽ��� ��ġ�� ù ī���� ��ġ��
	FVector Location = Cards[0]->GetActorLocation();
	SetActorLocation(Location, false, nullptr, ETeleportType::ResetPhysics);

	float Offset = 0.0f;

	// ���� ������ ���� Ʋ������ ī����� ��ġ ����
	for (int32 i = 0; i < Cards.Num(); i++)
	{
		if (bFalling)
		{
			Cast<ACard>(Cards[i])->GetVisualMesh()->SetSimulatePhysics(false);
			Offset = HeightOffset;
		}
		else {
			Offset = ZOffset;
		}
		FVector NewLocation = GetActorLocation();
		NewLocation.X += XOffset * i;
		NewLocation.Z += Offset * i;
		Cards[i]->SetActorLocation(NewLocation, false, nullptr, ETeleportType::ResetPhysics);
	}
}

bool ACardStack::GetIsSellable() const
{
	bool bSellable = true;
	for (AActor* CardActor : Cards)
	{
		ACard* Card = Cast<ACard>(CardActor);
		if (Card->GetCardPrice() == -1) 
		{
			bSellable = false;
			break;
		}
	}
	return bSellable;
}

int ACardStack::GetPriceSum() const
{
	int PriceSum = 0;
	for (AActor* CardActor : Cards)
	{
		ACard* Card = Cast<ACard>(CardActor);
		int Price = Card->GetCardPrice();
		if (Price != -1)
		{
			PriceSum += Price;
		}
	}
	return PriceSum;
}

TArray<ACard*> ACardStack::GetCardsByType(CardType Type) const
{
	TArray<ACard*> TargetCards;
	
	for (TObjectPtr<AActor> CardActor : Cards)
	{
		TObjectPtr<ACard> Card = Cast<ACard>(CardActor);
		if (Card->GetCardType() == Type)
		{
			TargetCards.Add(Card);
		}
	}

	return TargetCards;
}

bool ACardStack::GetIsCoinStack() const
{
	for (AActor* CardActor : Cards)
	{
		ACard* Card = Cast<ACard>(CardActor);
		if (Card->GetCardID() != 6) return false;
	}
	return true;
}

void ACardStack::BreakGame()
{
	for (AActor* CardActor : Cards)
	{
		TObjectPtr<ACard> Card = Cast<ACard>(CardActor);
		Card->BreakGame();
	}
}

void ACardStack::ResumeGame()
{
	for (AActor* CardActor : Cards)
	{
		TObjectPtr<ACard> Card = Cast<ACard>(CardActor);
		Card->ResumeGame();
	}
}

// Called every frame
void ACardStack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (CraftingRecipeID != -1 && SLGameMode->GetPlayState() == GamePlayState::PlayState)
	{
		CraftingProgress += DeltaTime;
		GetFirstCard()->UpdateProgressBar(CraftingProgress);
		if (CraftingProgress >= MakeTime)
		{
			if (CraftingRecipeID < 1000)
			{
				CompleteCrafting();
			}
			else {
				CompleteProducing();
			}
		}
	}


}

void ACardStack::AddCard(TArray<AActor*> NewCards)
{
	for (AActor* Card : NewCards)
	{
		AddCard(Card);
	}
}

// Add a card to the stack
void ACardStack::AddCard(AActor* CardActor)
{
	// UpdatePosition();

	ACard* Card = Cast<ACard>(CardActor);
	// ���콺 �̵��� �޴� �߿��� ������ �� ���� ��Ȱ��ȭ
	// ���� ī�尡 �߰��Ǿ�� �ϴµ� �� �ż� �ϴ� �ּ� ó��, ���� �������� �̻��ϸ� �� Ȱ��ȭ�ϱ��
	//if (!Card->GetVisualMesh()->IsSimulatingPhysics()) return;

	// Add the card to the stack
	Cards.Add(Card);
	int32 CardID = Card->GetCardID();
	if (CardCount.Contains(CardID))
	{
		CardCount[CardID] += 1;
	} else {
		CardCount.Add(CardID);
		CardCount[CardID] = 1;
	}

	if (Card)
	{
		AActor* OldStackActor = Card->GetCardStack();

		Card->SetCardStack(this);
		GetFirstCard()->UpdateWorldBorder(Cards.Num());

		if (OldStackActor != nullptr)
		{
			ACardStack* OldCardStack = Cast<ACardStack>(OldStackActor);

			// ���� �ڽ��� ���ÿ� ī�尡 �ϳ����̾����� ������ ����
			if (OldCardStack->Cards.Num() == 1)
			{
				OldCardStack->RemoveFromGamemode();
				OldStackActor->Destroy();
			}
			else {
				// �ƴϾ����� �ڽŸ� ����
				OldCardStack->Cards.Remove(Card);
			}
		}

	}

	UpdateCraftingRecipe();

	int Length = Cards.Num();
	// print the length to the screen
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Length: %d"), Length));

	if (Length == 1)
	{
		FVector CardLocation = Card->GetActorLocation();
		CardLocation.Z = 0.f;
		SetActorLocation(CardLocation);
	}
	else
	{
		// Set the cards location to the stack
		FVector NewLocation = GetActorLocation();
		NewLocation.X += XOffset * (Cards.Num() - 1);
		NewLocation.Z += ZOffset * (Cards.Num() - 1);
		Card->SetActorLocation(NewLocation);
	}


}

void ACardStack::RemoveCard(TArray<AActor*> NewCards, bool bDespawn)
{

	for (int32 i = 0; i < NewCards.Num(); i++)
	{
		RemoveCard(NewCards[i], bDespawn);
	}
}

void ACardStack::RemoveCard(int32 Index, bool bDespawn)
{
	RemoveCard(Cards[Index], bDespawn);
}

void ACardStack::RemoveCard(AActor* CardActor, bool bDespawn)
{
	ACard* Card = Cast<ACard>(CardActor);
	if (Card == nullptr) return;

	int32 CardID = Card->GetCardID();
	CardCount[CardID] -= 1;

	Cards.Remove(CardActor);
	Card->SetCardStack(nullptr);
	if (bDespawn)
	{
		CardActor->Destroy();
	}

	if (Cards.Num() > 0)
	{
		UpdateCraftingRecipe();
		GetFirstCard()->UpdateWorldBorder(Cards.Num());
	}
	else
	{
		RemoveFromGamemode();
		Destroy();
	}
}

void ACardStack::RemoveAllCards(bool bDespawn)
{
	RemoveCard(Cards, bDespawn);
}

AActor* ACardStack::FindMouseSender(FVector Location) const
{
	double CardX = Cards[0]->GetActorLocation().X;
	
	FVector Origin;
	FVector BoxExtent;
	Cards[0]->GetActorBounds(true, Origin, BoxExtent);
	const double CardHeight = BoxExtent.X;
	CardX += CardHeight;
	
	double HitLocationX = Location.X;
	
	int32 CardIndex = FMath::Floor((HitLocationX - CardX) / XOffset);
	if (CardIndex >= Cards.Num()) CardIndex = Cards.Num() - 1;
	if (CardIndex < 0) CardIndex = 0;
	return Cards[CardIndex];
}

void ACardStack::HandleStackMove(ACard* Sender, ECardMovement Movement)
{
	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (SLGameMode->GetPlayState() == GamePlayState::BreakState) return;

	// ���콺 �Է��� ���� ī�� ã��
	int32 SenderIndex = Cards.IndexOfByKey(Sender);

	// �ݺ��� �ʿ���� ������
	switch (Movement)
	{
	case ECardMovement::StartHover:
		UpdatePosition();
		break;
	case ECardMovement::EndHover:
		break;
	case ECardMovement::StartDrag:
		if (SenderIndex != 0)
		{
			SplitCardStack(this, SenderIndex);
		} else {
			SLGameMode->SetCardHighlight(true, this);
		}
		break;
	case ECardMovement::EndDrag:
		SLGameMode->SetCardHighlight(false);
		break;
	case ECardMovement::MoveToCursor:
		break;
	default:
		break;
	}

	// ī�帶�� �ݺ�
	for (int32 i = 0; i < Cards.Num(); i++)
	{
		AActor* Card = Cards[i];
		ACard* CardActor = Cast<ACard>(Card);
		if (CardActor)
		{
			switch (Movement)
			{
			case ECardMovement::StartHover:
				CardActor->StartHover(HoveringHeight + i * HeightOffset);
				break;
			case ECardMovement::EndHover:
				CardActor->EndHover();
				break;
			case ECardMovement::StartDrag:
				if (SenderIndex == 0)
				{
					CardActor->StartCardDrag();
				}
				break;
			case ECardMovement::EndDrag:
				CardActor->EndCardDrag();
				break;
			case ECardMovement::MoveToCursor:
				CardActor->MoveCardToCursor(FloatingHeight + i * HeightOffset);
				UpdatePosition();
				break;
			default:
				break;
			}
		}
	}
}

bool ACardStack::GetCardStackable(ACardStack* CardStack, ACardStack* OtherStack)
{
	// ī������ ������ �� ����
	if (CardStack->GetFirstCard()->GetCardType() == CardType::pack || OtherStack->GetFirstCard()->GetCardType() == CardType::pack)
	{
		return false;
	}

	// TODO: ��Ȯ�� ī�� ���� ���� ����
	
	// (����� ������ ī�� ID == �� ù ī�� ID�� true)
	// ACard* FirstCard = Cast<ACard>(CardStack->Cards[0]);
	// return (FirstCard->GetCardID() == OtherStack->GetLastCard()->GetCardID());

	// �ӽ�: �׻� true ��ȯ
	return true;
}

ACardStack* ACardStack::GetNearestStackable(float StackSearchDistance, ACardStack* ExceptionStack)
{
	float MinDistance = StackSearchDistance;
	ACardStack* NearestStackableStack = nullptr;

	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
	TArray<ACardStack*> AllCardStacks = SLGameMode->GetAllCardStacks();
	
	for (ACardStack* CardStack : AllCardStacks)
	{
		if (CardStack == this) continue;
		if (ExceptionStack != nullptr && ExceptionStack == CardStack) continue;

		float Distance = GetDistanceTo(CardStack);
		if (Distance <= MinDistance && CardStack->GetCardStackable(this, CardStack))
		{
			NearestStackableStack = CardStack;
			MinDistance = Distance;
		}
	}

	return NearestStackableStack;
}

// 1�� Index �������� ������� = 0�� Index���� ���� �ϳ�, 1�� Index���� ���� �ϳ�
void ACardStack::SplitCardStack(ACardStack* CardStack, int32 Index)
{
	// Index�� �ʹ� ũ�ų� ������ return
	if (Index >= CardStack->Cards.Num() || Index <= -1) return;

	TArray<AActor*> NewCards;

	for (int32 i = Index; i < CardStack->Cards.Num(); i++)
	{
		// FString CardName = CardStack->Cards[i]->GetName();
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%d"), i));
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, CardName);
		NewCards.Add(CardStack->Cards[i]);
	}

	CardStack->RemoveCard(NewCards);

	FVector Location = CardStack->GetActorLocation();

	AActor* NewCardStackActor = CardStack->GetWorld()->SpawnActor
	(
		ACardStack::StaticClass(),
		&Location
	);

	if (NewCardStackActor == nullptr) return;
	ACardStack* NewCardStack = Cast<ACardStack>(NewCardStackActor);

	for (int32 i = 0; i < NewCards.Num(); i++)
	{
		NewCardStack->AddCard(NewCards[i]);
	}
	
	NewCardStack->UpdatePosition();
	// �и��� ī���� �Ʒ��� ȣ�� ����
	CardStack->HandleStackMove(CardStack->GetFirstCard(), ECardMovement::EndHover);
	// �巡�� ������ ������ ��ġ �� ����
	NewCardStack->HandleStackMove(NewCardStack->GetFirstCard(), ECardMovement::StartDrag);
	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(CardStack));
	SLGameMode->SetCardHighlight(true, NewCardStack);
}

void ACardStack::HandleStackCollision(ACard* OtherCard)
{
	ACardStack* OtherCardStack = Cast<ACardStack>(OtherCard->GetCardStack());
	UpdatePosition(); OtherCardStack->UpdatePosition();

	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (SLGameMode->GetDraggingStack() == this && GetCardStackable(this, OtherCardStack))
		// ����
	{
		TArray<AActor*> NewCards;
		OtherCardStack->AddCard(Cards);
		// ������ ������ ȣ�� ����
		OtherCardStack->HandleStackMove(OtherCardStack->GetLastCard(), ECardMovement::EndHover);
	} else {
		// �浹
		FVector SelfVector, OtherVector;
		GetCardCollisionVector(OtherCard, SelfVector, OtherVector);
		PushCards(SelfVector);
		OtherCardStack->PushCards(OtherVector);
	}
}

void ACardStack::PushCards(FVector Force)
{
	for (AActor* Card : Cards)
	{
		ACard* CardActor = Cast<ACard>(Card);
		CardActor->Push(Force * FApp::GetDeltaTime());
	}
}

void ACardStack::GetCardCollisionVector(AActor* Other, FVector& SelfVector, FVector& OtherVector) const
{
	FVector ActorLocation = GetFirstCard()->GetActorLocation();
	FVector OtherLocation = Other->GetActorLocation();
	FVector CollisionVector = (ActorLocation - OtherLocation).GetSafeNormal(0.0001f);

	// ������ ��ȯ �� ���� �������� �浹
	if (CollisionVector.Size() < 0.0001f) {
		CollisionVector.X = FMath::SRand() - 0.5f;
		CollisionVector.Y = FMath::SRand() - 0.5f;
		CollisionVector = CollisionVector.GetSafeNormal(0.0001f);
	}

	CollisionVector.Z = 0.0f;
	CollisionVector *= Collsionforce;
	SelfVector = CollisionVector;
	CollisionVector *= OtherCollsionWeight;
	OtherVector = CollisionVector;
}

void ACardStack::SetShowProgressBar(bool NewShowProgressBar) const
{
	if (NewShowProgressBar)
	{
		GetFirstCard()->SetShowProgressBar(true);
	}
	else {
		for (AActor* CardActor : Cards)
		{
			Cast<ACard>(CardActor)->SetShowProgressBar(false);
		}
	}
}