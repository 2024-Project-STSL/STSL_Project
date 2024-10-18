// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterCard.h"
#include "Data/EquipmentData.h"
#include "PersonCard.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API APersonCard : public ACharacterCard
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	class UDataTable* EquipmentDataTable;

	UPROPERTY(EditAnyWhere, Category = "Charactor")
	int FoodEaten = 0;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	FEquipmentData Weapon;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* OverlapArea;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	FEquipmentData MainArmor;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	FEquipmentData SubArmor;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UWidgetComponent> EquipmentIndicator;

	TObjectPtr<AActor> MainActor;

	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	TObjectPtr<AActor> LastEquipment = nullptr;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	float CurrentEquipmentCooldown = -1.0f;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	float LastEquipmentCooldown = 5.0f;

protected:
	virtual void BeginPlay() override;

	void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void OnHit(UPrimitiveComponent* HitCompoent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

public:

	APersonCard();
	APersonCard(int32 CardID);

	bool Eat(TObjectPtr<ACard> Food, FCardAnimationCallback& Callback);

	UFUNCTION(BlueprintCallable)
	void CharacterDeath(EDeathReason DeathReason) override;

	UFUNCTION(BlueprintCallable)
	void ResetFood() { FoodEaten = 0; }

	UFUNCTION(BlueprintCallable)
	bool IsFull() const { return FoodEaten == CardData.AddTypeValue; }

	UFUNCTION(BlueprintCallable)
	FEquipmentData GetEquipment(EquipType TargetSlot);

	UFUNCTION(BlueprintCallable)
	void Equip(AActor* Card, bool bShowEquipment = true);
	void Equip(int CardCode, bool bShowEquipment = true);

	UFUNCTION(BlueprintCallable)
	void Unequip(EquipType TargetSlot);

	UFUNCTION(BlueprintCallable)
	void AddEquipmentStat(FEquipmentData Equipment);

	UFUNCTION(BlueprintCallable)
	void UpdateStat();
};
