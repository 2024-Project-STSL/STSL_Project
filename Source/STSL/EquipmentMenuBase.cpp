// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentMenuBase.h"
#include "Data/CardData.h"

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

    static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/CardDB.CardDB'"));
    if (DataTable.Succeeded())
    {
        CardDataTable = DataTable.Object;
    }
}

void UEquipmentMenuBase::SetShowEquipmentDetail(bool Showing)
{
    bShowEquipmentDetail = Showing;
    if (bShowEquipmentDetail)
    {
        GetWidgetFromName(TEXT("EquipmentDetail"))->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        ResetShowDetailTime();
    }
    else {
        GetWidgetFromName(TEXT("EquipmentDetail"))->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UEquipmentMenuBase::SetSlotIndicator(EquipType TargetSlot, bool Equipped) const
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

    if (Equipped)
    {
        SlotImage->SetBrush(CircleOn);
    }
    else {
        SlotImage->SetBrush(CircleOff);
    }
}

void UEquipmentMenuBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    CurrentShowDetailTime -= DeltaTime;
    if (bShowEquipmentDetail && CurrentShowDetailTime < 0)
    {
        SetShowEquipmentDetail(false);
    }
}

void UEquipmentMenuBase::UpdateDetail(int NewCode, UTextBlock* TargetText, UImage* TargetImage)
{
    if (NewCode == -1)
    {
        TargetText->SetText(FText::FromString(TEXT("")));
        TargetImage->SetBrush(FSlateBrush());
    }
    else {
        FName RowName = FName(*FString::FromInt(NewCode));
        FCardData* TargetData = CardDataTable->FindRow<FCardData>(RowName, TEXT(""));
        TargetText->SetText(FText::FromString(TargetData->CardName));
        FSlateFontInfo TargetFont = TargetText->GetFont();
        TargetFont.Size = FMath::Clamp(60.0f / TargetData->CardName.Len(), 1.0f, 10.0f);
        TargetText->SetFont(TargetFont);

        FString MaterialPath = "/Script/Engine.Texture2D'/Game/CardImages/";
        MaterialPath += FString::Printf(TEXT("%d.%d'"), TargetData->CardCode, TargetData->CardCode);
        // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, MaterialPath);
        UTexture2D* CardMaterial = LoadObject<UTexture2D>(nullptr, *MaterialPath);
        FSlateBrush CardBrush;
        CardBrush.SetResourceObject(CardMaterial);
        if (CardMaterial)
        {
            TargetImage->SetBrush(CardBrush);
        }
    }
}

void UEquipmentMenuBase::UpdateEquipmentMenu(int NewWeapon, int NewArmor, int NewSubArmor, bool bShowEquipment)
{

    Weapon = NewWeapon;
    Armor = NewArmor;
    SubArmor = NewSubArmor;

    if (bShowEquipment) SetShowEquipmentDetail(true);

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

    UpdateDetail(NewWeapon, WeaponName, WeaponImage);
    UpdateDetail(NewArmor, ArmorName, ArmorImage);
    UpdateDetail(NewSubArmor, SubArmorName, SubArmorImage);
}

void UEquipmentMenuBase::SendUnequip(EquipType TargetSlot) const
{
    OnUnequip.ExecuteIfBound(TargetSlot);
}
