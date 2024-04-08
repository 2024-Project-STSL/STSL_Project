// Fill out your copyright notice in the Description page of Project Settings.

#include "Card.h"
#include "CardStack.h"
#include "CraftingProgressBarWidget.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ACard::ACard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    VisualMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CardVisualAsset(TEXT("/Script/Engine.StaticMesh'/Game/Mesh/DummyCard.DummyCard'"));
    // static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> CardPhysicalMeterial(TEXT("/Script/PhysicsCore.PhysicalMaterial'/Game/Material/CardPhysicalMaterial1.CardPhysicalMaterial1'"));

    if (CardVisualAsset.Succeeded())
    {
        VisualMesh->SetStaticMesh(CardVisualAsset.Object);
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        VisualMesh->SetSimulatePhysics(true);
        VisualMesh->BodyInstance.bUseCCD = true;
        VisualMesh->BodyInstance.UpdateInstanceSimulatePhysics();
        VisualMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
        VisualMesh->SetNotifyRigidBodyCollision(true);
        VisualMesh->SetMassOverrideInKg(NAME_None, CardMass, true);
        //VisualMesh->SetPhysMaterialOverride(CardPhysicalMeterial.Object);
        VisualMesh->BodyInstance.bLockXRotation = true;
        VisualMesh->BodyInstance.bLockYRotation = true;
        VisualMesh->BodyInstance.bLockZRotation = true;
        //VisualMesh->BodyInstance.SetDOFLock(EDOFMode::SixDOF);
    }
   
    static ConstructorHelpers::FObjectFinder<UFont> HanbitFont(TEXT("/Script/Engine.Font'/Game/Fonts/Hanbit_Offline.Hanbit_Offline'"));

    if (HanbitFont.Succeeded())
    {
        CardFont = HanbitFont.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterial> HanbitFontMat(TEXT("/Script/Engine.Material'/Game/Fonts/Hanbit_Offline_Material.Hanbit_Offline_Material'"));

    if (HanbitFontMat.Succeeded())
    {
        CardFontMat = HanbitFontMat.Object;
    }

    TitleText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TitleText"));
    TitleText->SetHorizontalAlignment(EHTA_Center);
    TitleText->SetVerticalAlignment(EVRTA_TextCenter);
    TitleText->SetRelativeLocationAndRotation(FVector(247.5f, 0.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
    TitleText->SetTextRenderColor(FColor::Black);
    TitleText->SetWorldSize(FontSize);
    TitleText->SetMaterial(0, CardFontMat);
    TitleText->SetFont(CardFont);
    TitleText->SetText(FText::FromString(TEXT("Dummy")));
    TitleText->SetupAttachment(VisualMesh);

    SellPriceText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SellPriceText"));
    SellPriceText->SetHorizontalAlignment(EHTA_Center);
    SellPriceText->SetVerticalAlignment(EVRTA_TextCenter);
    SellPriceText->SetRelativeLocationAndRotation(FVector(-237.5f, -140.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
    SellPriceText->SetTextRenderColor(FColor::Black);
    SellPriceText->SetWorldSize(FontSize/1.5f);
    SellPriceText->SetMaterial(0, CardFontMat);
    SellPriceText->SetFont(CardFont);
    SellPriceText->SetText(FText::FromString(TEXT("99")));
    SellPriceText->SetupAttachment(VisualMesh);

    AddTypeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("AddTypeText"));
    AddTypeText->SetHorizontalAlignment(EHTA_Center);
    AddTypeText->SetVerticalAlignment(EVRTA_TextCenter);
    AddTypeText->SetRelativeLocationAndRotation(FVector(-237.5f, 140.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
    AddTypeText->SetTextRenderColor(FColor::Black);
    AddTypeText->SetWorldSize(FontSize/1.5f);
    AddTypeText->SetMaterial(0, CardFontMat);
    AddTypeText->SetFont(CardFont);
    AddTypeText->SetText(FText::FromString(TEXT("99")));
    AddTypeText->SetupAttachment(VisualMesh);
    
    CardImageWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ImageWidget"));
    CardImageWidget->SetupAttachment(VisualMesh);
    CardImageWidget->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
    // 스케일로 나누어 카드 전체의 스케일 변화에 대응
    CardImageWidget->SetDrawSize(FVector2D(375.0f / GetActorScale().X, 375.0f / GetActorScale().X));

    static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/CardDB.CardDB'"));
    if (DataTable.Succeeded())
    {
        CardDataTable = DataTable.Object;
    }

    CraftingProgressWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CraftingProgressWidget"));
    CraftingProgressWidget->SetupAttachment(VisualMesh);

    static ConstructorHelpers::FClassFinder<UUserWidget> CraftingProgressBarRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/CraftingProgressBar.CraftingProgressBar_C'"));
    if (CraftingProgressBarRef.Succeeded())
    {
        CraftingProgressWidget->SetWidgetClass(CraftingProgressBarRef.Class);
        CraftingProgressWidget->SetRelativeLocationAndRotation(FVector(330.0f, 0.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
        CraftingProgressWidget->SetDrawSize(FVector2D(400.0f, 50.0f));
        CraftingProgressWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    SetShowProgressBar(bShowProgressBar);

    LoadCard();
}

ACard::ACard(int32 CardID)
{
    ACard::ACard();
    this->CardData.CardCode = CardID;
}

void ACard::LoadCard()
{
    if (CardDataTable != nullptr)
    {
        FName RowName = FName(*FString::FromInt(CardData.CardCode));
        FCardData* RowData = CardDataTable->FindRow<FCardData>(RowName, TEXT(""));
        if (RowData != nullptr) CardData = *RowData;
    }
    TitleText->SetText(FText::FromString(CardData.CardName));
    TitleText->SetWorldSize(FMath::Clamp(FontSize*6/CardData.CardName.Len(), 10.0f, 80.0f));
    if (CardData.CardPrice >= 1)
    {
        SellPriceText->SetText(FText::AsNumber(CardData.CardPrice));
    } else {
        SellPriceText->SetText(FText::FromString(""));
    }

    if (CardData.AddType != AddType::nope)
    {
        AddTypeText->SetText(FText::AsNumber(CardData.AddTypeValue));
    } else {
        AddTypeText->SetText(FText::FromString(""));
    }

    // 동적으로 카드 이미지 로드
    if (CardData.CardCode)
    {
        FString MaterialPath = "/Script/Engine.Material'/Game/CardImages/";
        MaterialPath += FString::Printf(TEXT("%d_Mat.%d_Mat'"), CardData.CardCode, CardData.CardCode);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, MaterialPath);
        UMaterial* CardMaterial = LoadObject<UMaterial>(nullptr, *MaterialPath);
        if (CardMaterial)
        {
            CardImageWidget->SetMaterial(0, CardMaterial);
            CardImageWidget->RequestRedraw();
        }
    }
}

void ACard::SetShowProgressBar(bool NewShowProgressBar)
{
    bShowProgressBar = NewShowProgressBar;
    CraftingProgressWidget->SetVisibility(bShowProgressBar);
}

// Called when the game starts or when spawned
void ACard::BeginPlay()
{
	Super::BeginPlay();
    VisualMesh->OnComponentHit.AddDynamic(this, &ACard::OnHit);
}

// Called every frame
void ACard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACard::SetCardStack(AActor* Stack)
{
    CardStack = Stack; 
}

void ACard::SendMovementToStack(ECardMovement Movement)
{
    ACardStack* CardStackActor = Cast<ACardStack>(CardStack);
    CardStackActor->HandleStackMove(this, Movement);
}

void ACard::StartMouseHover()
{
    SendMovementToStack(ECardMovement::StartHover);
}
void ACard::EndMouseHover()
{
    SendMovementToStack(ECardMovement::EndHover);
}
void ACard::StartDrag()
{
    SendMovementToStack(ECardMovement::StartDrag);
}
void ACard::EndDrag()
{
    SendMovementToStack(ECardMovement::EndDrag);
}
void ACard::MoveToCursor()
{
    SendMovementToStack(ECardMovement::MoveToCursor);
}

void ACard::StartHover(float HoveringHeight) 
{
    VisualMesh->SetSimulatePhysics(false);
    FVector Location = GetActorLocation();
    Location.Z = HoveringHeight;
    SetActorLocation(Location);
}

void ACard::EndHover()
{
    VisualMesh->SetSimulatePhysics(true);
}

FVector ACard::GetMouseHitLocation() const
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    FHitResult HitResult;
    PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
    return HitResult.Location;
}

void ACard::StartCardDrag()
{
    VisualMesh->SetSimulatePhysics(false);
    FVector ActorLocation = GetActorLocation();
    FVector HitLocation = GetMouseHitLocation();
    CardOffset = ActorLocation - HitLocation;
}

void ACard::EndCardDrag()
{
    VisualMesh->SetSimulatePhysics(true);
}

void ACard::MoveCardToCursor(float FloatingHeight)
{
    FVector HitLocation = GetMouseHitLocation();
    FVector NewLocation = (CardOffset + HitLocation);
    NewLocation.Z = FloatingHeight;
    SetActorLocation(NewLocation, false, nullptr, ETeleportType::ResetPhysics);
}

void ACard::OnHit(UPrimitiveComponent* HitCompoent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor->IsA<ACard>())
    {
        ACardStack* CardStackActor = Cast<ACardStack>(CardStack);
        ACard* OtherCard = Cast<ACard>(OtherActor);

        // 같은 스택의 카드끼리는 충돌 처리 없음
        if (CardStack == OtherCard->CardStack) return;

        CardStackActor->HandleStackCollision(OtherCard);
    }
}

// 힘을 지정하지 않은 Push()는 카드 소환 튀어나오기 연출에 사용
void ACard::Push()
{
    float RandomX = FMath::RandRange(-1 * PushVector.X, PushVector.X);
    float RandomY = FMath::RandRange(-1 * PushVector.X, PushVector.X);
    FVector RandomPushVector = FVector(RandomX, RandomY, PushVector.Z);
    VisualMesh->AddImpulse(RandomPushVector);
}

void ACard::Push(FVector Force)
{
    if (VisualMesh->IsSimulatingPhysics())
    {
        SetActorLocation(GetActorLocation() + Force);
    }
}

void ACard::UpdateProgressBar(float Current, float Max)
{
    UCraftingProgressBarWidget* CraftingProgressBar = Cast<UCraftingProgressBarWidget>(CraftingProgressWidget->GetWidget());
    if (CraftingProgressBar != nullptr)
    {
        CraftingProgressBar->SetProgress(Current, Max);
    }
}

void ACard::UpdateProgressBar(float Current)
{
    UCraftingProgressBarWidget* CraftingProgressBar = Cast<UCraftingProgressBarWidget>(CraftingProgressWidget->GetWidget());
    if (CraftingProgressBar != nullptr)
    {
        CraftingProgressBar->SetProgress(Current);
    }
}