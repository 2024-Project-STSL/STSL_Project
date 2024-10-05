// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FieldManager.generated.h"

UCLASS()
class STSL_API AFieldManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Field")
	TMap<FString, TObjectPtr<AActor>> Walls;

	UPROPERTY(EditAnywhere, Category = "Field")
	TArray<TObjectPtr<AActor>> CardAreas;

	UPROPERTY(EditAnywhere, Category = "Field")
	TObjectPtr<AActor> Floor;

	UPROPERTY(EditAnywhere, Category = "Field")
	TObjectPtr<AActor> Floor2;

	UPROPERTY(VisibleAnywhere, Category = "World")
	TMap<FString, float> BaseWorldBorder;

	UPROPERTY(VisibleAnywhere, Category = "World")
	TMap<FString, float> WorldBorder;

	UPROPERTY(EditAnywhere, Category = "World")
	FVector BaseFloorScale = FVector(4.0f, 6.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "World")
	float ExtendRange = 500.0f;

	UPROPERTY(EditAnywhere, Category = "World")
	float WallOffset = 50.0f;

	UPROPERTY(EditAnywhere, Category = "World")
	float BuyAreaHeight = 800.0f;

public:	
	// Sets default values for this actor's properties
	AFieldManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Category = "Field")
	void SetField(int ExtendmentLevel);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "World")
	void SetWorldBorder(int CardLimit, int BaseCardLimit);

	UFUNCTION(BlueprintCallable, Category = "World")
	TMap<FString, float> GetWorldBorder(bool bExcludeBuyArea = false) const;

};
