// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentMenuBase.h"
#include <Components/Image.h>

UEquipmentMenuBase::UEquipmentMenuBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UTexture2D>CircleOnRef(TEXT("/Script/Engine.Texture2D'/Game/UI/CircleOn.CircleOn'"));

    if (CircleOnRef.Succeeded())
    {
        TObjectPtr<UTexture2D> CircleOnTexture = CircleOnRef.Object;
        CircleOn.SetResourceObject(CircleOnTexture);
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D>CircleOffRef(TEXT("/Script/Engine.Texture2D'/Game/UI/CircleOff.CircleOff'"));

    if (CircleOffRef.Succeeded())
    {
        TObjectPtr<UTexture2D> CircleOffTexture = CircleOffRef.Object;
        CircleOff.SetResourceObject(CircleOffTexture);
    }
}

void UEquipmentMenuBase::SetShowEquipmentDetail(bool Showing)
{
    bShowEquipmentDetail = Showing;
    if (bShowEquipmentDetail)
    {
        GetWidgetFromName(TEXT("EquipmentDetail"))->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
    else {
        GetWidgetFromName(TEXT("EquipmentDetail"))->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UEquipmentMenuBase::SetSlotIndicator(EquipType TargetSlot, bool Showing) const
{
    TObjectPtr<UImage> SlotImage = nullptr;

    switch (TargetSlot)
    {
    case EquipType::Weapon:
        SlotImage = Cast<UImage>(GetWidgetFromName(TEXT("WeaponIndicator")));
        break;
    case EquipType::MainArmor:
        SlotImage = Cast<UImage>(GetWidgetFromName(TEXT("ArmorIndicator")));
        break;
    case EquipType::SubArmor:
        SlotImage = Cast<UImage>(GetWidgetFromName(TEXT("SubArmorIndicator")));
        break;
    default:
        break;
    }

    if (Showing)
    {
        SlotImage->SetBrush(CircleOn);
    }
    else {
        SlotImage->SetBrush(CircleOff);
    }
}

void UEquipmentMenuBase::UpdateEquipmentMenu(int NewWeapon, int NewArmor, int NewSubArmor)
{
    Weapon = NewWeapon;
    Armor = NewArmor;
    SubArmor = NewSubArmor;

    if (Weapon != -1)
    {
        SetSlotIndicator(EquipType::Weapon, true);
    }
    else {
        SetSlotIndicator(EquipType::Weapon, false);
    }

    if (Armor != -1)
    {
        SetSlotIndicator(EquipType::MainArmor, true);
    }
    else {
        SetSlotIndicator(EquipType::MainArmor, false);
    }

    if (SubArmor != -1)
    {
        SetSlotIndicator(EquipType::SubArmor, true);
    }
    else {
        SetSlotIndicator(EquipType::SubArmor, false);
    }
}
