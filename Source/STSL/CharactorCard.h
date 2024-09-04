// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/DropData.h"
#include "Card.h"
#include "CharactorCard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, ACharactorCard*, DeadCard);

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

	UPROPERTY(EditAnywhere, Category = "Battle")
	EBattleState BattleState;

protected:
	virtual void BeginPlay() override;

	void LoadCard() override;

	// 장비, 입은 피해 등이 적용되기 전의 원본 스탯
	UPROPERTY(EditAnywhere, Category = "Charactor")
	FCharactorData CharactorBaseStat;

	UPROPERTY(EditAnywhere, Category = "Charactor")
	FCharactorData CharactorStat;

	UPROPERTY(EditAnywhere, Category = "Charactor")
	float MoveCooldown = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Charactor")
	float CurrentMoveCooldown = 0.0f;

	void Tick(float DeltaTime) override;

	void PushTowardPeople();

public:

	ACharactorCard();
	ACharactorCard(int32 CardID);

	void SendMovementToStack(ECardMovement Movement) override;

	void CharactorMove();

	void CharactorDrop();

	void CharactorDamage(int Damage);

	void CharactorDeath(EDeathReason Reason);

	UFUNCTION(BlueprintCallable, Category = "Charactor")
	FCharactorData GetCharactorStat() const { return CharactorStat; }

	UFUNCTION(BlueprintCallable, Category = "Battle")
	EBattleState GetBattleState() const { return BattleState; }

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void SetBattleState(EBattleState NewBattleState) { BattleState = NewBattleState; }

	FOnDeath OnDeath;
};
