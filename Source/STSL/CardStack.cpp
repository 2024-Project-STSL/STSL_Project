// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameModeBase.h"
#include "CardStack.h"
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
	for (int16 i = 0; i < Cards.Num(); i++)
	{
		FVector NewLocation = GetActorLocation();
		NewLocation.X += XOffset * (i - 1);
		NewLocation.Z += ZOffset * (i - 1);
		Cards[i]->SetActorLocation(NewLocation);
	}
}

// Called every frame
void ACardStack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Init card stack with two cards - deprecated
AActor* ACardStack::InitCard(AActor* Card, AActor* Other)
{
	if (Card->GetActorLocation().Z <= Other->GetActorLocation().Z)
	{
		AddCard(Card); AddCard(Other);
	}
	else 
	{
		AddCard(Other); AddCard(Card);
	}
	return Other;
}

// Add a card to the stack
void ACardStack::AddCard(AActor* Card)
{
	UpdatePosition();

	// Add the card to the stack
	Cards.Add(Card);
	ACard* CardActor = Cast<ACard>(Card);
	if (CardActor)
	{
		AActor* OldStackActor = CardActor->GetCardStack();

		LastCard = CardActor;
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
				// TODO: 아니었으면 자신만 삭제
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("First Card"));
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

void ACardStack::HandleStackMove(ECardMovement Movement)
{
	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (int16 i = 0; i < Cards.Num(); i++)
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
				CardActor->StartCardDrag();
				SLGameMode->SetCardHighlight(true, this);
				break;
			case ECardMovement::EndDrag:
				CardActor->EndCardDrag();
				SLGameMode->SetCardHighlight(false);
				break;
			case ECardMovement::MoveToCursor:
				CardActor->MoveCardToCursor(FloatingHeight + i * HeightOffset);
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
	return (FirstCard->GetCardID() == OtherStack->LastCard->GetCardID());
}

void ACardStack::HandleStackCollision(ACard* OtherCard)
{
	ACardStack* OtherCardStack = Cast<ACardStack>(OtherCard->GetCardStack());
	if (IsCardStackable(this, OtherCardStack))
		// 스택
	{
		for (AActor* NewCard : OtherCardStack->Cards)
		{
			AddCard(NewCard);
		}
	}
	// 충돌
	else {
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
		CardActor->Push(Force);
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