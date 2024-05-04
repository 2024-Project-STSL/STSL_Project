// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BuySellInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBuySellInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STSL_API IBuySellInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Category = "Card")
	virtual void BuyCard(ACardStack* CardStack);

	UFUNCTION(Category = "Card")
	virtual void SellCard(ACardStack* CardStack);
};
