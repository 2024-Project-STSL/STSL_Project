// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/DropData.h"
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

	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	UDataTable* DropTable;

	UPROPERTY(EditAnywhere, Category = "Crafting")
	float MaxDropCooldown = 80.0f;

	UPROPERTY(EditAnywhere, Category = "Crafting")
	float MinDropCooldown = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Crafting")
	float CurrentDropCooldown = 9999.9f;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UMaterialInstance* NeturalCardMaterial;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UMaterialInstance* EnemyCardMaterial;

protected:
	virtual void BeginPlay() override;

	void LoadCard() override;

	UPROPERTY(EditAnywhere, Category = "Charactor")
	float MoveCooldown = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Charactor")
	float CurrentMoveCooldown = 0.0f;

	UPROPERTY(EditAnyWhere, Category = "Charactor")
	int Health = 0;

	void Tick(float DeltaTime) override;

	void PushTowardPeople();

public:

	ACharactorCard();
	ACharactorCard(int32 CardID);

	void SendMovementToStack(ECardMovement Movement) override;

	void CharactorMove();

	void CharactorDrop();
};
