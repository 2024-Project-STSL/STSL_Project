// Fill out your copyright notice in the Description page of Project Settings.

#include "Card.h"
#include "CardStack.h"
#include "CraftingProgressBarWidget.h"
#include "SLGameModeBase.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ACard::ACard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = VisualMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CardVisualAsset(TEXT("/Script/Engine.StaticMesh'/Game/Mesh/DummyCard.DummyCard'"));
    // static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> CardPhysicalMeterial(TEXT("/Script/PhysicsCore.PhysicalMaterial'/Game/Material/CardPhysicalMaterial1.CardPhysicalMaterial1'"));

    if (CardVisualAsset.Succeeded())
    {
        VisualMesh->SetStaticMesh(CardVisualAsset.Object);
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        VisualMesh->SetSimulatePhysics(true);
        VisualMesh->BodyInstance.bUseCCD = true;
        VisualMesh->BodyInstance.UpdateInstanceSimulatePhysics();
        VisualMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
        VisualMesh->SetNotifyRigidBodyCollision(true);
        VisualMesh->SetMassOverrideInKg(NAME_None, CardMass, true);
        //VisualMesh->SetPhysMaterialOverride(CardPhysicalMeterial.Object);
        VisualMesh->BodyInstance.bLockXRotation = true;
        VisualMesh->BodyInstance.bLockYRotation = true;
        VisualMesh->BodyInstance.bLockZRotation = true;
        //VisualMesh->BodyInstance.SetDOFLock(EDOFMode::SixDOF);
    }
   
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> FoodMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Material/FoodCardMaterial.FoodCardMaterial'"));

    if (FoodMat.Succeeded())
    {
        FoodCardMat = FoodMat.Object;
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
    
    PriceIcon = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PriceIcon"));
    PriceIcon->SetHorizontalAlignment(EHTA_Center);
    PriceIcon->SetVerticalAlignment(EVRTA_TextCenter);
    PriceIcon->SetRelativeLocationAndRotation(FVector(-235.0f, -140.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
    PriceIcon->SetTextRenderColor(FColor::Black);
    PriceIcon->SetWorldSize(FontSize / 0.8f);
    PriceIcon->SetMaterial(0, CardFontMat);
    PriceIcon->SetFont(CardFont);
    PriceIcon->SetText(FText::FromString(TEXT("○")));
    PriceIcon->SetupAttachment(VisualMesh);

    HealthIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthIcon"));
    HealthIcon->SetupAttachment(VisualMesh);
    HealthIcon->SetRelativeLocationAndRotation(FVector(-240.0f, 140.0f, 0.55f), FRotator(90.0f, 0.0f, 180.0f));
    // 스케일로 나누어 카드 전체의 스케일 변화에 대응
    HealthIcon->SetDrawSize(FVector2D(90.0f / GetActorScale().X, 90.0f / GetActorScale().X));

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

    static ConstructorHelpers::FObjectFinder<UDataTable> CharDataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/CharactorDB.CharactorDB'"));
    if (CharDataTable.Succeeded())
    {
        CharactorDataTable = CharDataTable.Object;
    }

    CraftingProgressWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CraftingProgressWidget"));
    CraftingProgressWidget->SetupAttachment(VisualMesh);

    static ConstructorHelpers::FClassFinder<UUserWidget> CraftingProgressBarRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/CraftingProgressBar.CraftingProgressBar_C'"));
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
        if (CardData.IsCharactor())
        {
            FCharactorData* CharRowData = CharactorDataTable->FindRow<FCharactorData>(RowName, TEXT(""));
            Health = CharRowData->CharHealth;
        }
    }
    TitleText->SetText(FText::FromString(CardData.CardName));
    TitleText->SetWorldSize(FMath::Clamp(FontSize * 6.0f / CardData.CardName.Len(), FontSize / 8.0f, FontSize));
    if (CardData.CardPrice >= 0)
    {
        SellPriceText->SetText(FText::AsNumber(CardData.CardPrice));
    } else {
        SellPriceText->SetText(FText::FromString(""));
        PriceIcon->SetText(FText::FromString(""));
    }

    if (CardData.AddType != AddType::nope)
    {
        AddTypeText->SetText(FText::AsNumber(CardData.AddTypeValue));
        if (CardData.AddType == AddType::foodable)
        {
            VisualMesh->SetMaterial(0, FoodCardMat);
        }
    } 
    else {
        AddTypeText->SetText(FText::FromString(""));
    }

    if (CardData.IsCharactor())
    {
        AddTypeText->SetText(FText::AsNumber(Health));
        // TODO : 하트 아이콘으로 변경
        FString MaterialPath = "/Script/Engine.Material'/Game/CardImages/14_Mat.14_Mat'";
        // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, MaterialPath);
        UMaterial* HealthIconMaterial = LoadObject<UMaterial>(nullptr, *MaterialPath);
        if (HealthIconMaterial)
        {
            HealthIcon->SetMaterial(0, HealthIconMaterial);
            HealthIcon->RequestRedraw();
        }
    }
    else {
    }
    
    // 동적으로 카드 이미지 로드
    if (CardData.CardCode)
    {
        FString MaterialPath = "/Script/Engine.Material'/Game/CardImages/";
        MaterialPath += FString::Printf(TEXT("%d_Mat.%d_Mat'"), CardData.CardCode, CardData.CardCode);
        // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, MaterialPath);
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

void ACard::SetAddTypeValue(int Value)
{
    CardData.AddTypeValue = Value;
    if (CardData.AddTypeValue == 0)
    {
        if (CardData.AddType == AddType::dropable)
        {
            Cast<ACardStack>(CardStack)->RemoveCard(this, true);
        }
    }
}

// Called when the game starts or when spawned
void ACard::BeginPlay()
{
	Super::BeginPlay();
    VisualMesh->OnComponentHit.AddDynamic(this, &ACard::OnHit);
    ResetWorldBorder();
}

// Called every frame
void ACard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (bPreventDragging && TargetLocation != FVector::ZeroVector)
    {
        FVector OriginLocation;
        FVector Location = OriginLocation = GetActorLocation();
        Location += TargetFollowSpeed * DeltaTime * (TargetLocation - Location);
        if ((Location - OriginLocation).Length() < 0.1f)
        {
            TargetCallback.ExecuteIfBound();
        }
        else {
            SetActorLocation(Location);
        }
    }
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
    if (bPreventDragging || bFloating) return;
    SendMovementToStack(ECardMovement::StartHover);
}
void ACard::EndMouseHover()
{
    SendMovementToStack(ECardMovement::EndHover);
}
void ACard::StartDrag()
{
    if (bPreventDragging) return;
    SendMovementToStack(ECardMovement::StartDrag);
}
void ACard::EndDrag()
{
    SendMovementToStack(ECardMovement::EndDrag);
}
void ACard::MoveToCursor()
{
    if (bPreventDragging) return;
    SendMovementToStack(ECardMovement::MoveToCursor);
}

void ACard::StartHover(float HoveringHeight) 
{
    bFloating = true;
    VisualMesh->SetSimulatePhysics(false);
    FVector Location = GetActorLocation();
    Location.Z = HoveringHeight;
    SetActorLocation(Location, false, nullptr, ETeleportType::ResetPhysics);
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
    bFloating = true;
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
    NewLocation.X = FMath::Clamp(NewLocation.X, WorldBorder["Down"], WorldBorder["Up"]);
    NewLocation.Y = FMath::Clamp(NewLocation.Y, WorldBorder["Left"], WorldBorder["Right"]);
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
    else if (OtherActor->GetRootComponent()->GetName() == TEXT("CardAreaMesh"))
    {
        ACardStack* CardStackActor = Cast<ACardStack>(CardStack);
        Cast<IBuySellInterface>(OtherActor)->SellCard(CardStackActor);
        Cast<IBuySellInterface>(OtherActor)->BuyCard(CardStackActor);
    }
    else if (OtherActor->GetName() == TEXT("Floor"))
    {
        UpdateGroundPosition();
    }
}

// 힘을 지정하지 않은 Push()는 카드 소환 튀어나오기 연출에 사용
void ACard::Push()
{
    Push(PushVector);
}

void ACard::Push(FVector Force)
{
    float RandomX = FMath::RandRange(Force.X / 2.0f, Force.X);
    float RandomY = FMath::RandRange(Force.Y / 2.0f, Force.Y);
    RandomX *= FMath::RandBool() ? 1 : -1;
    RandomY *= FMath::RandBool() ? 1 : -1;
    FVector RandomPushVector = FVector(RandomX, RandomY, Force.Z);
    VisualMesh->AddImpulse(RandomPushVector);
    bFloating = true;
}

void ACard::Move(FVector Movement)
{
    FVector NewLocation = GetActorLocation() + Movement;
    NewLocation.X = FMath::Clamp(NewLocation.X, WorldBorderWithoutBuyArea["Down"], WorldBorderWithoutBuyArea["Up"]);
    NewLocation.Y = FMath::Clamp(NewLocation.Y, WorldBorderWithoutBuyArea["Left"], WorldBorderWithoutBuyArea["Right"]);
    SetActorLocation(NewLocation, false, nullptr, ETeleportType::ResetPhysics);
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

void ACard::UpdateGroundPosition()
{
    ACardStack* CardStackActor = Cast<ACardStack>(CardStack);
    if (bFloating)
    {
        FVector NewLocation, OldLocation;
        NewLocation = OldLocation = GetActorLocation();
        NewLocation.X = FMath::Clamp(NewLocation.X, WorldBorderWithoutBuyArea["Down"], WorldBorderWithoutBuyArea["Up"]);
        NewLocation.Y = FMath::Clamp(NewLocation.Y, WorldBorderWithoutBuyArea["Left"], WorldBorderWithoutBuyArea["Right"]);

        // 만약 좌표가 Clamp되었으면
        if (FMath::Abs(NewLocation.X - OldLocation.X) > 0.001f || FMath::Abs(NewLocation.Y - OldLocation.Y) > 0.001f)
        {
            // 살짝 띄운 곳에 이동시켜 충돌 처리 등이 이루어지게
            NewLocation.Z = 10.0f;
            SetActorLocation(NewLocation, false, nullptr, ETeleportType::ResetPhysics);
            VisualMesh->SetAllPhysicsLinearVelocity(FVector::Zero());
            CardStackActor->UpdatePosition(false);
        }
        else {
            NewLocation.Z = 0.0f;
            SetActorLocation(NewLocation, false, nullptr, ETeleportType::ResetPhysics);
            CardStackActor->UpdatePosition(true);
            bFloating = false;
        }
        
        ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
        if (SLGameMode->GetDraggingStack() == CardStack)
        {
            SLGameMode->EmptyDraggingStack();
        }
    }
}

void ACard::UpdateWorldBorder(int Length)
{
    ACardStack* CardStackActor = Cast<ACardStack>(CardStack);

    FVector Origin;
    FVector BoxExtent;
    GetActorBounds(true, Origin, BoxExtent);

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
    
    ResetWorldBorder();
    WorldBorder["Down"] -= CardStackActor->GetXOffset() * (Length - 1);
    WorldBorderWithoutBuyArea["Down"] -= CardStackActor->GetXOffset() * (Length - 1);

    if (GetActorLocation().X < WorldBorder["Down"])
    {
        FVector NewLocation = GetActorLocation();
        NewLocation.X = FMath::Clamp(NewLocation.X, WorldBorderWithoutBuyArea["Down"], WorldBorderWithoutBuyArea["Up"]);

        // 살짝 띄운 곳에 이동시켜 충돌 처리 등이 이루어지게
        NewLocation.Z = 10.0f;
        SetActorLocation(NewLocation, false, nullptr, ETeleportType::ResetPhysics);
        CardStackActor->UpdatePosition(false);
    }
}

void ACard::ResetWorldBorder()
{
    FVector Origin;
    FVector BoxExtent;
    GetActorBounds(true, Origin, BoxExtent);

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

    WorldBorder = SLGameMode->GetWorldBorder(false);
    WorldBorderWithoutBuyArea = SLGameMode->GetWorldBorder(true);

    WorldBorder["Left"] += BoxExtent.Y;
    WorldBorder["Right"] -= BoxExtent.Y;
    WorldBorder["Down"] += BoxExtent.X;
    WorldBorder["Up"] -= BoxExtent.X;

    WorldBorderWithoutBuyArea["Left"] += BoxExtent.Y;
    WorldBorderWithoutBuyArea["Right"] -= BoxExtent.Y;
    WorldBorderWithoutBuyArea["Down"] += BoxExtent.X;
    WorldBorderWithoutBuyArea["Up"] -= BoxExtent.X;
}

void ACard::BreakGame()
{
    bPreventDragging = true;
    bPhysicsBeforeBreak = VisualMesh->IsSimulatingPhysics();
    VisualMesh->SetSimulatePhysics(false);
}

void ACard::ResumeGame()
{
    bPreventDragging = false;
    VisualMesh->SetSimulatePhysics(bPhysicsBeforeBreak);
}

void ACard::MoveBack(FCardAnimationCallback& Callback)
{
    TargetLocation = MovedLocation;
    TargetCallback = Callback;
}

void ACard::MoveToAnother(ACard* OtherCard, FCardAnimationCallback& Callback)
{
    MovedLocation = GetActorLocation();
    TargetLocation = OtherCard->GetActorLocation();
    TargetCallback = Callback;
}

bool ACard::Eat(TObjectPtr<ACard> Food, FCardAnimationCallback& Callback)
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

    FoodEaten += Food->CardData.AddTypeValue;
    Food->CardData.AddTypeValue -= RequireFood;
    Food->MoveToAnother(this, TargetCallback);
    return true;

}
