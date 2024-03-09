// Fill out your copyright notice in the Description page of Project Settings.


#include "CardStack.h"

// Sets default values
ACardStack::ACardStack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACardStack::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACardStack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Init card stack with two cards
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
	// Add the card to the stack
	Cards.Add(Card);

	int Length = Cards.Num();
	// print the length to the screen
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Length: %d"), Length));

	if (Length == 1)
	{
		FVector CardLocation = Card->GetActorLocation();
		Location = CardLocation;
		Location.Z = 0.f;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("First Card"));
	}
	else
	{
		// Set the cards location to the stack
		FVector NewLocation = Location;
		NewLocation.X += XOffset * (Cards.Num() - 1);
		NewLocation.Z += ZOffset * (Cards.Num() - 1);
		Card->SetActorLocation(NewLocation);
	}


}

