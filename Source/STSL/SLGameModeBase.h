// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CardStack.h"
#include "SLGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API ASLGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TArray<ACardStack*> CardStacks;

	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	bool IsCardHighlight;

public:
	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void AddCardStack(ACardStack* CardStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void SetCardHighlight(bool CardHighlight, ACardStack* HoveringStack = nullptr);
};
