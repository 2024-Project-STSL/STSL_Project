// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/DropData.h"
#include "Card.h"
#include "CharacterCard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, ACharacterCard*, DeadCard);

/**
 * 
 */
UENUM(BlueprintType)
enum class EBattleState : uint8
{
	Idle = 0,
	Wait,
	Ready,
	Select,
	Attack,
	Hit,
	Dead
};

UENUM(BlueprintType)
enum class EDeathReason : uint8
{
	Hunger = 0,
	Damaged,
	Unknown
};

UENUM(BlueprintType)
enum class ECharacterMovement : uint8
{
	Static = 0,
	RandomJump,
	FollowHuman
};

UCLASS()
class STSL_API ACharacterCard : public ACard
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	class UDataTable* CharacterDataTable;

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

	UPROPERTY(EditAnywhere, Category = "Battle")
	EBattleState BattleState;

protected:
	virtual void BeginPlay() override;

	void LoadCard() override;

	// 장비, 입은 피해 등이 적용되기 전의 원본 스탯
	UPROPERTY(EditAnywhere, Category = "Character")
	FCharacterData CharacterBaseStat;

	UPROPERTY(EditAnywhere, Category = "Character")
	FCharacterData CharacterStat;

	UPROPERTY(EditAnywhere, Category = "Character")
	float MoveCooldown = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Character")
	float CurrentMoveCooldown = 0.0f;

	void Tick(float DeltaTime) override;

	void PushTowardPeople();

public:

	ACharacterCard();
	ACharacterCard(int32 CardID);

	void SendMovementToStack(ECardMovement Movement) override;

	void CharacterMove();

	void CharacterDrop();

	void CharacterDamage(int Damage);

	void CharacterDeath(EDeathReason Reason);

	UFUNCTION(BlueprintCallable, Category = "Character")
	FCharacterData GetCharacterStat() const { return CharacterStat; }

	UFUNCTION(BlueprintCallable, Category = "Battle")
	EBattleState GetBattleState() const { return BattleState; }

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void SetBattleState(EBattleState NewBattleState);

	FOnDeath OnDeath;
};
