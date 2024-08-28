// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/EquipmentData.h"
#include "EquipmentMenuBase.generated.h"

/**
 * 
 */

UCLASS()
class STSL_API UEquipmentMenuBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnyWhere, Category = "Equipment")
	bool bShowEquipmentDetail = false;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	float ShowDetailTime = 30.0f;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	float CurrentShowDetailTime = -1.0f;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	int Weapon = -1;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	int Armor = -1;

	UPROPERTY(EditAnyWhere, Category = "Equipment")
	int SubArmor = -1;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush CircleOn;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush CircleOff;

public:

	UEquipmentMenuBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetShowEquipmentDetail(bool Showing);

	UFUNCTION(BlueprintCallable)
	void SetSlotIndicator(EquipType TargetSlot, bool Showing) const;

	UFUNCTION(BlueprintCallable)
	void UpdateEquipmentMenu(int NewWeapon, int NewArmor, int NewSubArmor);
};
