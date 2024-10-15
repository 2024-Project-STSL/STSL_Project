// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <SLSaveGame.h>
#include "SLGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API USLGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	USLGameInstance();

	bool bLoadGame = false;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Setting")
	float MasterVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Setting")
	float BackgroundVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Setting")
	float EffectVolume = 1.0f;
	
	UFUNCTION(BlueprintCallable)
	void SetLoadGame(bool LoadGame) { bLoadGame = LoadGame; }
	
	UFUNCTION(BlueprintCallable)
	bool IsLoadGame() const { return bLoadGame; }

	UFUNCTION(BlueprintCallable)
	void SaveSettings();
};
