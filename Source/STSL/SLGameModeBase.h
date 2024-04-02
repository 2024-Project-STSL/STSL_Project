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

	// ���� �ֱ� ���콺�� ���� �巡�� ���۵� ����
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	ACardStack* DraggingStack;

	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	bool bIsCardHighlight;

public:

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void AddCardStack(ACardStack* CardStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void RemoveCardStack(ACardStack* CardStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void SetCardHighlight(bool bCardHighlight, ACardStack* NewDraggingStack = nullptr);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	ACardStack* GetDraggingStack() const { return DraggingStack; };
};
