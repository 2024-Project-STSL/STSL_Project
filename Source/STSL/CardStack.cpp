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
		int ReqCardCount = 0; // 조합에 사용된 카드의 총 수량
		for (int i = 0; i < ReqAmount; i++)
		{
			int ReqCardCode = Recipe->ReqCardCode[i];
			int ReqCardAmount = Recipe->ReqValue[i];

			if (!CardCount.Contains(ReqCardCode)) break;
			if (CardCount[ReqCardCode] != ReqCardAmount) break;

			Amount++;
			ReqCardCount += ReqCardAmount;
		}

		// 조합법 외의 잡 카드가 있으면 조합법 무효
		return (ReqAmount == Amount && ReqCardCount == Cards.Num());
	}
	else {
		return false;
	}
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

	SetShowProgressBar(bIsCrafting);
	if (!bIsCrafting)
	{
		CraftingRecipeID = -1;
	}

	if (bIsRecipeChanged)
	{
		// TODO : 각 카드 별로 제작 시간 변경

		// 제작 진행도 초기화
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
		// 같은 종류의 카드는 전부 사라지거나 전부 안 사라지므로,
		// 레시피 대신 카드 별로 순회
		// 카드가 사라지며 카드 index가 깨지지 않게 하기 위해 역순으로 순회
		for (int i = Cards.Num()-1; i >= 0; i--)
		{
			ACard* Card = Cast<ACard>(Cards[i]);
			int RecipeIndex = RecipeData->ReqCardCode.IndexOfByKey<int>(Card->GetCardID());
			if (RecipeData->bIsDeleted[RecipeIndex])
			{
				RemoveCard(i, true);
			}
		}

		// 카드는 '튀어나와야' 하므로, 사라지지 않은 남은 재료 스택과 쌓임이 보장되지 않음
		// 따라서 새 스택을 생성해서 결과물 투여
		FVector Location = GetActorLocation();

		ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
		ACardStack* NewCardStack = SLGameMode->SpawnCard(Location, RecipeData->CardCode);
		if (NewCardStack != nullptr)
		{
			Cast<ACard>(NewCardStack->Cards[0])->Push();
		}
	}

}

void ACardStack::UpdatePosition()
{
	if (Cards.Num() == 0) return;

	// 자신의 위치를 첫 카드의 위치로
	FVector Location = Cards[0]->GetActorLocation();
	SetActorLocation(Location);

	// 물리 엔진에 의해 틀어지는 카드들의 위치 보정
	for (int32 i = 0; i < Cards.Num(); i++)
	{
		FVector NewLocation = GetActorLocation();
		NewLocation.X += XOffset * i;
		NewLocation.Z += ZOffset * i;
		Cards[i]->SetActorLocation(NewLocation);
	}
}

// Called every frame
void ACardStack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CraftingRecipeID != -1)
	{
		CraftingProgress += DeltaTime;
		GetFirstCard()->UpdateProgressBar(CraftingProgress);
		if (CraftingProgress >= MakeTime)
		{
			CompleteCrafting();
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
	// 마우스 이동을 받는 중에는 합쳐질 수 없게 비활성화
	// 가끔 카드가 추가되어야 하는데 안 돼서 일단 주석 처리, 추후 움직임이 이상하면 재 활성화하기로
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

		if (OldStackActor != nullptr)
		{
			ACardStack* OldCardStack = Cast<ACardStack>(OldStackActor);

			// 원래 자신의 스택에 카드가 하나뿐이었으면 스택을 삭제
			if (OldCardStack->Cards.Num() == 1)
			{
				OldCardStack->RemoveFromGamemode();
				OldStackActor->Destroy();
			}
			else {
				// 아니었으면 자신만 삭제
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
		RemoveCard(NewCards[i]);
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
	}
	else
	{
		RemoveFromGamemode();
		Destroy();
	}
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
	// 마우스 입력을 보낸 카드 찾기
	int32 SenderIndex = Cards.IndexOfByKey(Sender);

	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

	// 반복이 필요없는 구문들
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

	// 카드마다 반복
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
	// TODO: 정확한 카드 스택 조건 구현
	
	// (상대의 마지막 카드 ID == 내 첫 카드 ID면 true)
	// ACard* FirstCard = Cast<ACard>(CardStack->Cards[0]);
	// return (FirstCard->GetCardID() == OtherStack->GetLastCard()->GetCardID());

	// 임시: 항상 true 반환
	return true;
}

// 1번 Index 기준으로 나누어라 = 0번 Index까지 스택 하나, 1번 Index부터 스택 하나
void ACardStack::SplitCardStack(ACardStack* CardStack, int32 Index)
{
	// Index가 너무 크거나 작으면 return
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
	// 분리된 카드의 아래쪽 호버 해제
	CardStack->HandleStackMove(CardStack->GetFirstCard(), ECardMovement::EndHover);
	// 드래그 시작한 스택의 위치 재 보정
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
		// 스택
	{
		TArray<AActor*> NewCards;
		OtherCardStack->AddCard(Cards);
		// 합쳐진 스택의 호버 해제
		OtherCardStack->HandleStackMove(OtherCardStack->GetLastCard(), ECardMovement::EndHover);
	} else {
		// 충돌
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