// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card.h"
#include "CharactorCard.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECharactorMovement : uint8
{
	Static = 0,
	RandomJump,
	FollowHuman
};

UCLASS()
class STSL_API ACharactorCard : public ACard
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	class UDataTable* CharactorDataTable;

	ACharactorCard();
	ACharactorCard(int32 CardID);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UMaterialInstance* NeturalCardMaterial;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UMaterialInstance* EnemyCardMaterial;

protected:
	void LoadCard() override;

	UPROPERTY(EditAnywhere, Category = "Charactor")
	float MoveCooldown = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Charactor")
	float CurrentMoveCooldown = 0.0f;

	UPROPERTY(EditAnyWhere, Category = "Charactor")
	int FoodEaten = 0;

	UFUNCTION(BlueprintCallable)
	void ResetFoodEaten() { FoodEaten = 0; }

	UPROPERTY(EditAnyWhere, Category = "Charactor")
	int Health = 0;

	void Tick(float DeltaTime) override;

	void PushTowardPeople();

public:
	void SendMovementToStack(ECardMovement Movement) override;

	void CharactorMove();

	bool Eat(TObjectPtr<ACard> Food, FCardAnimationCallback& Callback);

	void ResetFood() { FoodEaten = 0; }
};
