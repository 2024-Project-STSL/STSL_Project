// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <SLSaveGame.h>
#include "SLGameInstance.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAsyncSavingCompleted);

UCLASS()
class STSL_API USLGameInstance : public UGameInstance
{
	GENERATED_BODY()

	USLGameInstance();

	bool bLoadGame = false;

	bool bIsSaving = false;

	bool bPendingSaveSetting = false;

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

	UFUNCTION(BlueprintCallable)
	void SaveGame(USLSaveGame* SaveGame);

	UFUNCTION(BlueprintCallable)
	bool IsSaving() const { return bIsSaving; }

	UFUNCTION()
	void HandleSaveComplete(FString& SlotName, int32 UserIndex, bool bSuccess);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAsyncSavingCompleted OnAsyncSavingCompleted;

};
