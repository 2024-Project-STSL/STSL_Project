// Fill out your copyright notice in the Description page of Project Settings.


#include "PersonCard.h"
#include "SLGameModeBase.h"
#include "EquipmentMenuBase.h"
#include "EquipmentWidgetComponent.h"
#include "Card.h"
#include <Kismet/GameplayStatics.h>

APersonCard::APersonCard()
{
    
    EquipmentIndicator = CreateDefaultSubobject<UEquipmentWidgetComponent>(TEXT("EquipmentWidget"));
    EquipmentIndicator->SetupAttachment(VisualMesh);

    OverlapArea = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Overlap"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CardOverlapAsset(TEXT("/Script/Engine.StaticMesh'/Game/Mesh/DummyCard.DummyCard'"));
    // static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> CardPhysicalMeterial(TEXT("/Script/PhysicsCore.PhysicalMaterial'/Game/Material/CardPhysicalMaterial1.CardPhysicalMaterial1'"));

    if (CardOverlapAsset.Succeeded())
    {
        OverlapArea->SetStaticMesh(CardOverlapAsset.Object);
        OverlapArea->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
        OverlapArea->BodyInstance.UpdateInstanceSimulatePhysics();
        OverlapArea->SetRelativeScale3D(FVector(1.0f, 1.0f, 100.0f));
        OverlapArea->SetNotifyRigidBodyCollision(true);
        OverlapArea->BodyInstance.bLockXRotation = true;
        OverlapArea->BodyInstance.bLockYRotation = true;
        OverlapArea->BodyInstance.bLockZRotation = true;
        OverlapArea->SetVisibility(false);
        OverlapArea->SetCollisionProfileName(TEXT("OverlapAll"));
    }
    OverlapArea->SetupAttachment(VisualMesh);

    static ConstructorHelpers::FClassFinder<UUserWidget> CraftingProgressBarRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/EquipMentMenu.EquipMentMenu_C'"));
    if (CraftingProgressBarRef.Succeeded())
    {
        EquipmentIndicator->SetWidgetClass(CraftingProgressBarRef.Class);
        EquipmentIndicator->SetRelativeLocationAndRotation(FVector(-387.5f, 0.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
        EquipmentIndicator->SetDrawSize(FVector2D(400.0f, 265.5f));
        EquipmentIndicator->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
    OverlapArea->OnComponentBeginOverlap.AddDynamic(this, &APersonCard::OnOverlapBegin);
    TObjectPtr<UEquipmentMenuBase> EquipmentMenu = Cast<UEquipmentMenuBase>(EquipmentIndicator->GetWidget());
    EquipmentMenu->SetShowEquipmentDetail(false);
    EquipmentMenu->OnUnequip.BindUFunction(this, TEXT("Unequip"));
    MainActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMainActor::StaticClass());
}

void APersonCard::OnHit(UPrimitiveComponent* HitCompoent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor->IsA<ACard>())
    {
        ACard* OtherCard = Cast<ACard>(OtherActor);
        if (OtherCard->GetCardType() == CardType::equip && (OtherCard != LastEquipment || CurrentEquipmentCooldown < 0.0f))
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

void APersonCard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CurrentEquipmentCooldown -= DeltaTime;
}

void APersonCard::Equip(AActor* Card)
{
    TObjectPtr<ACard> CardActor = Cast<ACard>(Card);

    if (CardActor->GetCardType() != CardType::equip) return;

    int NewEquipID = CardActor->GetCardID();

    FName RowName = FName(*FString::FromInt(NewEquipID));
    FEquipmentData* EquipRowData = EquipmentDataTable->FindRow<FEquipmentData>(RowName, TEXT(""));
    FEquipmentData OldEquipment;

    switch (EquipRowData->EquipType)
    {
    case EquipType::Weapon:
        OldEquipment = Weapon;
        Weapon = *EquipRowData;
        break;
    case EquipType::MainArmor:
        OldEquipment = MainArmor;
        MainArmor = *EquipRowData;
        break;
    case EquipType::SubArmor:
        OldEquipment = SubArmor;
        SubArmor = *EquipRowData;
        break;
    default:
        break;
    }

    Cast<UEquipmentMenuBase>(EquipmentIndicator->GetWidget())->UpdateEquipmentMenu(Weapon.CardCode, MainArmor.CardCode, SubArmor.CardCode);
    UpdateStat();

    CardActor->Remove();

    if (OldEquipment.CardCode != -1)
    {
        FVector TargetCardLocation = GetActorLocation();
        TargetCardLocation.Z += 30.0f;

        ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
        ACardStack* TargetStack = SLGameMode->SpawnCard(TargetCardLocation, OldEquipment.CardCode);
        TargetStack->GetFirstCard()->Push(FVector(-4000.0f, 0.0f, 3000.0f), true);

        LastEquipment = TargetStack->GetFirstCard();
        CurrentEquipmentCooldown = LastEquipmentCooldown;
    }
}

void APersonCard::Unequip(EquipType TargetSlot)
{
    int TargetID = -1;
    FVector TargetOffset = FVector::Zero();

    switch (TargetSlot)
    {
    case EquipType::Weapon:
        TargetID = Weapon.CardCode;
        TargetOffset = FVector(-315.0f, -100.0f, 50.0f);
        Weapon.CardCode = -1;
        break;
    case EquipType::MainArmor:
        TargetID = MainArmor.CardCode;
        TargetOffset = FVector(-315.0f, 100.0f, 50.0f);
        MainArmor.CardCode = -1;
        break;
    case EquipType::SubArmor:
        TargetID = SubArmor.CardCode;
        TargetOffset = FVector(-315.0f, 100.0f, 50.0f);
        SubArmor.CardCode = -1;
        break;
    default:
        TargetID = -1;
        break;
    }

    if (TargetID == -1) return;

    Cast<UEquipmentMenuBase>(EquipmentIndicator->GetWidget())->UpdateEquipmentMenu(Weapon.CardCode, MainArmor.CardCode, SubArmor.CardCode);
    UpdateStat();

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
    FVector TargetCardLocation = GetActorLocation() + TargetOffset;
    ACardStack* TargetStack = SLGameMode->SpawnCard(TargetCardLocation, TargetID);

    SendMovementToStack(ECardMovement::EndDrag);
    FVector NewLocation = GetActorLocation();
    NewLocation.Z = 1.0f;
    SetActorLocation(NewLocation, false, nullptr, ETeleportType::ResetPhysics);

    TargetStack->GetFirstCard()->SendMovementToStack(ECardMovement::StartDrag);
    Cast<AMainActor>(MainActor)->SetMouseDraggingActor(TargetStack->GetFirstCard());

}

void APersonCard::AddEquipmentStat(FEquipmentData Equipment)
{
    switch (Equipment.StatType)
    {
    case StatType::Attack:
        CharacterStat.CharAttack += Weapon.StatValue;
        break;
    case StatType::Defence:
        CharacterStat.CharDefence += Weapon.StatValue;
        break;
    case StatType::Speed:
        CharacterStat.CharSpeed += Weapon.StatValue;
        break;
    default:
        break;
    }
}

void APersonCard::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA<ACard>())
    {
        ACard* OtherCard = Cast<ACard>(OtherActor);
        if (OtherCard->GetCardType() == CardType::equip)
        {
            Cast<UEquipmentMenuBase>(EquipmentIndicator->GetWidget())->SetShowEquipmentDetail(true);
        }
    }
}

void APersonCard::UpdateStat()
{
    int OldHealth = CharacterStat.CharHealth;
    CharacterStat = CharacterBaseStat;
    CharacterStat.CharHealth = OldHealth;

    if (Weapon.CardCode != -1) AddEquipmentStat(Weapon);
    if (MainArmor.CardCode != -1) AddEquipmentStat(MainArmor);
    if (SubArmor.CardCode != -1) AddEquipmentStat(SubArmor);
}