// Fill out your copyright notice in the Description page of Project Settings.


#include "MainActor.h"
#include "Card.h"

// Sets default values
AMainActor::AMainActor()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMainActor::SetMouseDraggingActor(AActor* NewMouseDraggingActor)
{
	MouseDraggingActor = NewMouseDraggingActor;
	if (MouseDraggingActor.IsA(ACard::StaticClass()))
	{
		SetMouseDraggingType(EMouseHitObject::Card);
	}
}

