// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharactorCard.h"
#include "PersonCard.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API APersonCard : public ACharactorCard
{
	GENERATED_BODY()

	UPROPERTY(EditAnyWhere, Category = "Charactor")
	int FoodEaten = 0;

public:

	APersonCard();
	APersonCard(int32 CardID);

	bool Eat(TObjectPtr<ACard> Food, FCardAnimationCallback& Callback);

	UFUNCTION(BlueprintCallable)
	void ResetFood() { FoodEaten = 0; }
};
