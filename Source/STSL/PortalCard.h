// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card.h"
#include "PortalCard.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API APortalCard : public ACard
{
	GENERATED_BODY()

	APortalCard();
	APortalCard(int32 CardID);

	UPROPERTY(EditAnywhere, Category = "Portal")
	int SpawnCount = 0;

	UPROPERTY(EditAnywhere, Category = "Crafting")
	float MaxDropCooldown = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Crafting")
	float CurrentDropCooldown = 9999.9f;

	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	UDataTable* DropTable;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UMaterialInstance* PortalCardMaterial;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SendMovementToStack(ECardMovement Movement);

	void SpawnEnemy();
};
