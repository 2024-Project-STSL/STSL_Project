// Fill out your copyright notice in the Description page of Project Settings.


#include "PersonCard.h"
#include "SLGameModeBase.h"
#include "EquipmentMenuBase.h"
#include "Card.h"
#include <Kismet/GameplayStatics.h>

APersonCard::APersonCard()
{
    EquipmentIndicator = CreateDefaultSubobject<UWidgetComponent>(TEXT("EquipmentWidget"));
    EquipmentIndicator->SetupAttachment(VisualMesh);
    
    static ConstructorHelpers::FClassFinder<UUserWidget> CraftingProgressBarRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/EquipMentMenu.EquipMentMenu_C'"));
    if (CraftingProgressBarRef.Succeeded())
    {
        EquipmentIndicator->SetWidgetClass(CraftingProgressBarRef.Class);
        EquipmentIndicator->SetRelativeLocationAndRotation(FVector(-387.5f, 0.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
        EquipmentIndicator->SetDrawSize(FVector2D(400.0f, 265.5f));
        EquipmentIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    static ConstructorHelpers::FObjectFinder<UDataTable> EquipDataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/EquipmentDB.EquipmentDB'"));
    if (EquipDataTable.Succeeded())
    {
        EquipmentDataTable = EquipDataTable.Object;
    }

    Weapon.CardCode = -1;
    MainArmor.CardCode = -1;
    SubArmor.CardCode = -1;
}

void APersonCard::BeginPlay()
{
    Super::BeginPlay();
    Cast<UEquipmentMenuBase>(EquipmentIndicator->GetWidget())->SetShowEquipmentDetail(false);
}

void APersonCard::OnHit(UPrimitiveComponent* HitCompoent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor->IsA<ACard>())
    {
        ACard* OtherCard = Cast<ACard>(OtherActor);
        if (OtherCard->GetCardType() == CardType::equip)
        {
            Equip(OtherCard);
            return;
        }
    }
    Super::OnHit(HitCompoent, OtherActor, OtherComp, NormalImpulse, Hit);
}

APersonCard::APersonCard(int32 CardID)
{
    APersonCard();
    this->CardData.CardCode = CardID;
}

bool APersonCard::Eat(TObjectPtr<ACard> Food, FCardAnimationCallback& Callback)
{
    if (CardData.CardType != CardType::person) return false;

    // 사람 카드의 AddTypeValue를 요구 식량으로 사용
    int FoodValue = Food->GetAddTypeValue();
    int RequireFood = GetAddTypeValue();

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
    TargetCallback.BindUObject(SLGameMode, &ASLGameModeBase::EatCompleted);

    if (FoodEaten >= RequireFood)
    {
        TargetCallback.ExecuteIfBound();
        return true;
    }

    FoodEaten += Food->GetAddTypeValue();
    Food->SetAddTypeValue(Food->GetAddTypeValue() - RequireFood);
    Food->MoveToAnother(this, TargetCallback);
    return true;

}

void APersonCard::Equip(AActor* Card)
{
    TObjectPtr<ACard> CardObject = Cast<ACard>(Card);

    if (CardObject->GetCardType() != CardType::equip) return;

    int NewEquipID = CardObject->GetCardID();

    FName RowName = FName(*FString::FromInt(NewEquipID));
    FEquipmentData* EquipRowData = EquipmentDataTable->FindRow<FEquipmentData>(RowName, TEXT(""));

    switch (EquipRowData->EquipType)
    {
    case EquipType::Weapon:
        Weapon = *EquipRowData;
        break;
    case EquipType::MainArmor:
        MainArmor = *EquipRowData;
        break;
    case EquipType::SubArmor:
        SubArmor = *EquipRowData;
        break;
    default:
        break;
    }

    Cast<UEquipmentMenuBase>(EquipmentIndicator->GetWidget())->UpdateEquipmentMenu(Weapon.CardCode, MainArmor.CardCode, SubArmor.CardCode);
    UpdateStat();

    CardObject->Remove();
}

void APersonCard::AddEquipmentStat(FEquipmentData Equipment)
{
    switch (Equipment.StatType)
    {
    case StatType::Attack:
        CharactorStat.CharAttack += Weapon.StatValue;
        break;
    case StatType::Defence:
        CharactorStat.CharDefence += Weapon.StatValue;
        break;
    case StatType::Speed:
        CharactorStat.CharSpeed += Weapon.StatValue;
        break;
    default:
        break;
    }
}

void APersonCard::UpdateStat()
{
    CharactorStat = CharactorBaseStat;
    AddEquipmentStat(Weapon);
    AddEquipmentStat(MainArmor);
    AddEquipmentStat(SubArmor);
}