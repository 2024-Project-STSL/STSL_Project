// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include <SLGameStateBase.h>
#include "SLSaveGame.generated.h"

USTRUCT(Atomic, BlueprintType)
struct STSL_API FCardStackSaveData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FVector CardLocation;

	UPROPERTY()
	TArray<FCardData> CardData;

};

/**
 * 
 */
UCLASS()
class STSL_API USLSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	USLSaveGame();

	UPROPERTY()
	float MasterVolume;

	UPROPERTY()
	float BackgroundVolume;

	UPROPERTY()
	float EffectVolume;

	UPROPERTY(BlueprintReadWrite, Category = "GameState")
	int32 Day;

	UPROPERTY()
	int32 Time;

	UPROPERTY()
	int32 CardLimit;

	UPROPERTY()
	GamePlayState CurrentPlayState;

	UPROPERTY()
	TArray<FCardStackSaveData> AllCardStacks;
};
