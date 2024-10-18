// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/EquipmentData.h"
#include "Components/TextBlock.h"
#include <Components/Image.h>
#include "EquipmentMenuBase.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUnequip, EquipType, EquipSlot);

UCLASS()
class STSL_API UEquipmentMenuBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	class UDataTable* CardDataTable;

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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ArmorName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ArmorImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SubArmorName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SubArmorImage;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:

	UEquipmentMenuBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetShowEquipmentDetail(bool Showing);

	UFUNCTION(BlueprintCallable)
	bool GetShowEquipmentDetail() const { return bShowEquipmentDetail; }

	UFUNCTION(BlueprintCallable)
	void ResetShowDetailTime() { CurrentShowDetailTime = ShowDetailTime; }

	UFUNCTION(BlueprintCallable)
	void SetSlotIndicator(EquipType TargetSlot, bool Equipped) const;

	UFUNCTION(BlueprintCallable)
	void UpdateDetail(int NewCode, UTextBlock* TargetText, UImage* TargetImage);

	UFUNCTION(BlueprintCallable)
	void UpdateEquipmentMenu(int NewWeapon, int NewArmor, int NewSubArmor, bool bShowEquipment = true);

	UFUNCTION(BlueprintCallable)
	void SendUnequip(EquipType TargetSlot) const;

	FOnUnequip OnUnequip;
};
