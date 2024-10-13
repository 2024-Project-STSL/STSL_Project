// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include <SLGameStateBase.h>
#include "SLSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API USLSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	USLSaveGame();

	UPROPERTY(BlueprintReadWrite, Category = "GameState")
	int32 Day;

	UPROPERTY()
	int32 Time;

	UPROPERTY()
	int32 CardLimit;

	UPROPERTY()
	GamePlayState CurrentPlayState;
};
