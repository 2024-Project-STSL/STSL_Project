// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCard.h"
#include "SLGameModeBase.h"
#include <Kismet/GameplayStatics.h>

ACharacterCard::ACharacterCard()
{
    CurrentMoveCooldown = MoveCooldown;

    HealthIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthIcon"));
    HealthIcon->SetupAttachment(VisualMesh);
    HealthIcon->SetRelativeLocationAndRotation(FVector(-240.0f, 140.0f, 0.55f), FRotator(90.0f, 0.0f, 180.0f));
    // 스케일로 나누어 카드 전체의 스케일 변화에 대응
    HealthIcon->SetDrawSize(FVector2D(90.0f / GetActorScale().X, 90.0f / GetActorScale().X));

    static ConstructorHelpers::FObjectFinder<UDataTable> CharDataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/CharacterDB.CharacterDB'"));
    if (CharDataTable.Succeeded())
    {
        CharacterDataTable = CharDataTable.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInstance> NeturalMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Material/NeturalCardMaterial.NeturalCardMaterial'"));
    if (NeturalMat.Succeeded())
    {
        NeturalCardMaterial = NeturalMat.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInstance> EnemyMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Material/EnemyCardMaterial.EnemyCardMaterial'"));
    if (EnemyMat.Succeeded())
    {
        EnemyCardMaterial = EnemyMat.Object;
    }

    static ConstructorHelpers::FObjectFinder<UDataTable> DropDataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DropDB.DropDB'"));
    if (DropDataTable.Succeeded())
    {
        DropTable = DropDataTable.Object;
    }
}

ACharacterCard::ACharacterCard(int32 CardID)
{
	ACharacterCard();
	this->CardData.CardCode = CardID;
}

void ACharacterCard::BeginPlay()
{
    Super::BeginPlay();
    CurrentDropCooldown = FMath::RandRange(MinDropCooldown, MaxDropCooldown);
}

void ACharacterCard::LoadCard()
{
    Super::LoadCard();

    FName RowName = FName(*FString::FromInt(CardData.CardCode));
    FCharacterData* CharRowData = CharacterDataTable->FindRow<FCharacterData>(RowName, TEXT(""));
    CharacterBaseStat = *CharRowData;
    CharacterStat = *CharRowData;

    if (CardData.CardType == CardType::netural)
    {
        VisualMesh->SetMaterial(0, NeturalCardMaterial);
    }

    if (CardData.CardType == CardType::enemy)
    {
        VisualMesh->SetMaterial(0, EnemyCardMaterial);
    }

    AddTypeText->SetText(FText::AsNumber(CharacterStat.CharHealth));
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

void ACharacterCard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

    if (SLGameMode->GetPlayState() != GamePlayState::PlayState) return;

    CurrentMoveCooldown -= DeltaTime;
    CurrentDropCooldown -= DeltaTime;
    if (CurrentMoveCooldown < 0)
    {
        CurrentMoveCooldown = MoveCooldown;
        CharacterMove();
    }

    if (CurrentDropCooldown < 0)
    {
        CurrentDropCooldown = FMath::RandRange(MinDropCooldown, MaxDropCooldown);
        CharacterDrop();
    }
}

void ACharacterCard::CharacterDrop()
{
    if (CardData.CardType != CardType::netural) return;

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

    FVector Location = GetActorLocation();

    Location.Z += 10;

    for (const auto& Row : DropTable->GetRowMap())
    {
        int RecipeID = FCString::Atoi(*Row.Key.ToString());

        const uint8* RowData = Row.Value;

        FDropData* DropData = reinterpret_cast<FDropData*>(const_cast<uint8*>(RowData));
        if (DropData->CardCode == GetCardID())
        {
            // TODO: 여러 종류의 카드를 drop하는 중립 몬스터 추가 시 수정 필요
            ACardStack* NewCardStack = SLGameMode->SpawnCard(Location, DropData->DropCardCode[0]);
            NewCardStack->GetFirstCard()->Push();
            break;
        }
    }
}

void ACharacterCard::CharacterDeath(EDeathReason Reason)
{
    OnDeath.Broadcast(this);
    Remove();
}

void ACharacterCard::CharacterDamage(int Damage)
{
    CharacterStat.CharHealth -= Damage;
    AddTypeText->SetText(FText::AsNumber(CharacterStat.CharHealth));

    if (CharacterStat.CharHealth <= 0) CharacterDeath(EDeathReason::Damaged);
}

void ACharacterCard::CharacterMove()
{
    switch (CardData.CardType)
    {
    case CardType::person:
        return;
    case CardType::netural:
        Push();
        break;
    case CardType::enemy:
        PushTowardPeople();
        break;
    default:
        break;
    }
}

void ACharacterCard::SendMovementToStack(ECardMovement Movement)
{
    if (CardData.CardType == CardType::person)
    {
        Super::SendMovementToStack(Movement);
    }
}

void ACharacterCard::PushTowardPeople()
{
    float MinDistance = 999999.9f;
    ACard* NearestPerson = nullptr;

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
    TArray<ACardStack*> AllCardStacks = SLGameMode->GetAllCardStacks();
    TArray<ACard*> Cards;

    for (ACardStack* CurrentCardStack : AllCardStacks)
    {
        Cards = CurrentCardStack->GetCardsByType(CardType::person);

        for (ACard* Card : Cards)
        {
            float Distance = GetDistanceTo(Card);
            if (Distance < MinDistance)
            {
                NearestPerson = Card;
                MinDistance = Distance;
            }
        }
    }
    
    if (NearestPerson == nullptr) return;

    FVector MoveVector = (NearestPerson->GetActorLocation() - GetActorLocation());
    MoveVector.Normalize();
    MoveVector *= PushVector.Size();
    MoveVector.Z = PushVector.Z;
    Push(MoveVector, true);
}

void ACharacterCard::SetBattleState(EBattleState NewBattleState)
{
    BattleState = NewBattleState;
}