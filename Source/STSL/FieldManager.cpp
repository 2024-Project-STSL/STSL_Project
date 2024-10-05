// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldManager.h"

// Sets default values
AFieldManager::AFieldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseWorldBorder.Add(TEXT("Left"), -3000.0f);
	BaseWorldBorder.Add(TEXT("Right"), 3000.0f);
	BaseWorldBorder.Add(TEXT("Down"), -2000.0f);
	BaseWorldBorder.Add(TEXT("Up"), 2000.0f);
	WorldBorder = BaseWorldBorder;

}

// Called when the game starts or when spawned
void AFieldManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFieldManager::SetWorldBorder(int CardLimit, int BaseCardLimit)
{
	int ExtendmentLevel = FMath::FloorToInt((CardLimit - BaseCardLimit) / 10.0f);
	float BorderExtendment = ExtendmentLevel * ExtendRange / 2.0f;

	WorldBorder = BaseWorldBorder;
	WorldBorder["Up"] += BorderExtendment;
	WorldBorder["Down"] -= BorderExtendment;
	WorldBorder["Left"] -= BorderExtendment;
	WorldBorder["Right"] += BorderExtendment;

	SetField(ExtendmentLevel);
}

void AFieldManager::SetField(int ExtendmentLevel)
{
	FVector NewFieldScale = BaseFloorScale;
	NewFieldScale.X += 0.5f * ExtendmentLevel;
	NewFieldScale.Y += 0.5f * ExtendmentLevel;

	Floor->SetActorScale3D(NewFieldScale);
	
	NewFieldScale.X = 0.8f;
	Floor2->SetActorScale3D(NewFieldScale);

	FVector NewFieldLocation = Floor2->GetActorLocation();
	NewFieldLocation.X = 1600.0f + 250.0f * ExtendmentLevel;

	Floor2->SetActorLocation(NewFieldLocation);

	for (TObjectPtr<AActor> CardArea : CardAreas)
	{
		NewFieldLocation = CardArea->GetActorLocation();
		NewFieldLocation.X = 1600.0f + 250.0f * ExtendmentLevel;

		CardArea->SetActorLocation(NewFieldLocation);
	}

	// 벽 설정 시작

	NewFieldScale = Walls["Down"]->GetActorScale3D();
	NewFieldLocation = Walls["Down"]->GetActorLocation();

	NewFieldScale.Y = 60.0f + 5.0f * ExtendmentLevel;
	NewFieldLocation.X = - (2000.0f + WallOffset + 250.0f * ExtendmentLevel);

	Walls["Down"]->SetActorScale3D(NewFieldScale);
	Walls["Down"]->SetActorLocation(NewFieldLocation);

	NewFieldScale = Walls["Right"]->GetActorScale3D();
	NewFieldLocation = Walls["Right"]->GetActorLocation();

	NewFieldScale.X = 40.0f + 5.0f * ExtendmentLevel;
	NewFieldLocation.Y = 3000.0f + WallOffset + 250.0f * ExtendmentLevel;

	Walls["Right"]->SetActorScale3D(NewFieldScale);
	Walls["Right"]->SetActorLocation(NewFieldLocation);

	NewFieldScale = Walls["Left"]->GetActorScale3D();
	NewFieldLocation = Walls["Left"]->GetActorLocation();

	NewFieldScale.X = 40.0f + 5.0f * ExtendmentLevel;
	NewFieldLocation.Y = - (3000.0f + WallOffset + 250.0f * ExtendmentLevel);

	Walls["Left"]->SetActorScale3D(NewFieldScale);
	Walls["Left"]->SetActorLocation(NewFieldLocation);

	// 벽 설정 종료
}

TMap<FString, float> AFieldManager::GetWorldBorder(bool bExcludeBuyArea) const
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