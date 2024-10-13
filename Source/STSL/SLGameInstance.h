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

protected:

public:
	
	UFUNCTION(BlueprintCallable)
	void SetLoadGame(bool LoadGame) { bLoadGame = LoadGame; }
	
	UFUNCTION(BlueprintCallable)
	bool IsLoadGame() const { return bLoadGame; }

};
