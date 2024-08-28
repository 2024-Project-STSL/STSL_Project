// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharactorCard.h"
#include "Data/EquipmentData.h"
#include "PersonCard.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API APersonCard : public ACharactorCard
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	class UDataTable* EquipmentDataTable;

	UPROPERTY(EditAnyWhere, Category = "Charactor")
	int FoodEaten = 0;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	FEquipmentData Weapon;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	FEquipmentData MainArmor;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	FEquipmentData SubArmor;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UWidgetComponent> EquipmentIndicator;

protected:
	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitCompoent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

public:

	APersonCard();
	APersonCard(int32 CardID);

	bool Eat(TObjectPtr<ACard> Food, FCardAnimationCallback& Callback);

	UFUNCTION(BlueprintCallable)
	void ResetFood() { FoodEaten = 0; }

	UFUNCTION(BlueprintCallable)
	void Equip(AActor* Card);

	UFUNCTION(BlueprintCallable)
	void AddEquipmentStat(FEquipmentData Equipment);

	UFUNCTION(BlueprintCallable)
	void UpdateStat();
};
