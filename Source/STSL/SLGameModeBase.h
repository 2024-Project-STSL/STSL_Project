// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CardStack.h"
#include "Card.h"
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

	// 가장 최근 마우스에 의해 드래그 시작된 스택
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	ACardStack* DraggingStack;

	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	bool bIsCardHighlight;

	UPROPERTY(VisibleAnywhere, Category = "World")
	TMap<FString, float> WorldBorder;

	UPROPERTY(EditAnywhere, Category = "World")
	float BuyAreaHeight = 800.0f;

public:
	ASLGameModeBase();

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void AddCardStack(ACardStack* CardStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void RemoveCardStack(ACardStack* CardStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	TArray<ACardStack*> GetAllCardStacks() const { return CardStacks; }

	UFUNCTION(BlueprintCallable, Category = "World")
	TMap<FString, float> GetWorldBorder(bool bExcludeBuyArea = false) const;

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void SetCardHighlight(bool bCardHighlight, ACardStack* NewDraggingStack = nullptr);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	ACardStack* GetDraggingStack() const { return DraggingStack; };

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void EmptyDraggingStack() { DraggingStack = nullptr; };

	UFUNCTION(BlueprintCallable, Category = "CardStack", BlueprintPure = false)
	ACardStack* SpawnCard(FVector Location, int CardID);
};
