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
}

// Add a card to the stack
void ACardStack::AddCard(AActor* Card)
{
	UpdatePosition();

	ACard* CardActor = Cast<ACard>(Card);
	// 마우스 이동을 받는 중에는 합쳐질 수 없게 비활성화
	if (!CardActor->GetVisualMesh()->IsSimulatingPhysics()) return;

	// Add the card to the stack
	Cards.Add(Card);

	if (CardActor)
	{
		AActor* OldStackActor = CardActor->GetCardStack();

		CardActor->SetCardStack(this);

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

	int Length = Cards.Num();
	// print the length to the screen
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Length: %d"), Length));

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

void ACardStack::RemoveCard(int32 Index, bool bDespawn)
{
	RemoveCard(Cards[Index], bDespawn);
}

void ACardStack::RemoveCard(AActor* CardActor, bool bDespawn)
{
	ACard* Card = Cast<ACard>(CardActor);
	if (Card == nullptr) return;

	Cards.Remove(CardActor);
	if (bDespawn)
	{
		CardActor->Destroy();
	}
}

int32 ACardStack::FindMouseSender() const
{
	double CardX = Cards[0]->GetActorLocation().X;
	
	// 임시
	const double CardHeight = -160.0;
	CardX -= CardHeight;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
	
	double HitLocationX = HitResult.Location.X;
	
	int32 CardIndex = FMath::Floor((HitLocationX - CardX) / XOffset);
	if (CardIndex >= Cards.Num()) CardIndex = Cards.Num() - 1;
	if (CardIndex < 0) CardIndex = 0;
	return CardIndex;
}

void ACardStack::HandleStackMove(ACard* Sender, ECardMovement Movement)
{
	// 마우스 입력을 보낸 카드 찾기
	int32 SenderIndex = FindMouseSender();

	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

	// 스택 나누는 구문은 카드 반복 for문 밖으로
	if (Movement == ECardMovement::StartDrag && SenderIndex != 0)
	{
		SplitCardStack(this, SenderIndex);
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
				UpdatePosition();
				CardActor->StartHover(HoveringHeight + i * HeightOffset);
				break;
			case ECardMovement::EndHover:
				CardActor->EndHover();
				break;
			case ECardMovement::StartDrag:
				CardActor->StartCardDrag();
				if (SenderIndex == 0)
				{
					SLGameMode->SetCardHighlight(true, this);
				}
				break;
			case ECardMovement::EndDrag:
				CardActor->EndCardDrag();
				SLGameMode->SetCardHighlight(false);
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

bool ACardStack::IsCardStackable(ACardStack* CardStack, ACardStack* OtherStack)
{
	// TODO: 정확한 카드 스택 조건 구현
	// 지금은 (상대의 마지막 카드 ID == 내 첫 카드 ID면 true)
	ACard* FirstCard = Cast<ACard>(CardStack->Cards[0]);
	return (FirstCard->GetCardID() == OtherStack->GetLastCard()->GetCardID());
}

// 1번 Index 기준으로 나누어라 = 0번 Index까지 스택 하나, 1번 Index부터 스택 하나
void ACardStack::SplitCardStack(ACardStack* CardStack, int32 Index)
{
	// Index가 너무 크거나 작으면 return
	if (Index >= CardStack->Cards.Num() || Index <= -1) return;

	TArray<AActor*> NewCards;

	for (int32 i = Index; i < CardStack->Cards.Num(); i++)
	{
		FString CardName = CardStack->Cards[i]->GetName();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%d"), i));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, CardName);
		NewCards.Add(CardStack->Cards[i]);
	}

	for (int32 i = 0; i < NewCards.Num(); i++)
	{
		CardStack->RemoveCard(NewCards[i]);
	}


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
		NewCardStack->Cards.Add(NewCards[i]);
		Cast<ACard>(NewCards[i])->SetCardStack(NewCardStackActor);
	}
	
	NewCardStack->UpdatePosition();
	// 분리된 카드의 아래쪽 호버 해제
	CardStack->HandleStackMove(CardStack->GetLastCard(), ECardMovement::EndHover);
	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(CardStack));
	SLGameMode->SetCardHighlight(true, NewCardStack);
}

void ACardStack::HandleStackCollision(ACard* OtherCard)
{
	ACardStack* OtherCardStack = Cast<ACardStack>(OtherCard->GetCardStack());
	UpdatePosition(); OtherCardStack->UpdatePosition();

	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (SLGameMode->GetHoveringStack() == this && IsCardStackable(this, OtherCardStack))
		// 스택
	{
		TArray<AActor*> NewCards;

		for (AActor* NewCard : Cards)
		{
			NewCards.Add(NewCard);
		}
		for (AActor* NewCard : NewCards)
		{
			OtherCardStack->AddCard(NewCard);
		}

		
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
	FVector ActorLocation = Cards[0]->GetActorLocation();
	FVector OtherLocation = Other->GetActorLocation();
	FVector CollisionVector = (ActorLocation - OtherLocation).GetSafeNormal(0.0001f);
	CollisionVector.Z = 0.0f;
	CollisionVector *= Collsionforce;
	SelfVector = CollisionVector;
	CollisionVector *= OtherCollsionWeight;
	OtherVector = CollisionVector;
}