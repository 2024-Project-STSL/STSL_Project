// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalCard.h"
#include <Kismet/GameplayStatics.h>
#include "SLGameModeBase.h"

APortalCard::APortalCard()
{
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> EnemyMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Material/EnemyCardMaterial.EnemyCardMaterial'"));
    if (EnemyMat.Succeeded())
    {
        PortalCardMaterial = EnemyMat.Object;
    }

    static ConstructorHelpers::FObjectFinder<UDataTable> DropDataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DropDB.DropDB'"));
    if (DropDataTable.Succeeded())
    {
        DropTable = DropDataTable.Object;
    }
}

APortalCard::APortalCard(int32 CardID)
{
	APortalCard();
	this->CardData.CardCode = CardID;
}

void APortalCard::BeginPlay()
{
    Super::BeginPlay();
    
    VisualMesh->SetMaterial(0, PortalCardMaterial);

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
    SpawnCount = SLGameMode->GetPortalSpawnCount();

    CurrentDropCooldown = MaxDropCooldown;
    SetShowProgressBar(true);
    UpdateProgressBar(CurrentDropCooldown, MaxDropCooldown);
}

void APortalCard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

    if (SLGameMode->GetPlayState() != GamePlayState::PlayState) return;

    CurrentDropCooldown -= DeltaTime;
    UpdateProgressBar(MaxDropCooldown - CurrentDropCooldown);
    if (CurrentDropCooldown < 0)
    {
        CurrentDropCooldown = MaxDropCooldown;
        SpawnEnemy();
    }
}

void APortalCard::SendMovementToStack(ECardMovement Movement)
{
    return;
}

void APortalCard::SpawnEnemy()
{

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

    FVector Location = GetActorLocation();

    Location.Z += 10;

    for (const auto& Row : DropTable->GetRowMap())
    {
        int RecipeID = FCString::Atoi(*Row.Key.ToString());

        const uint8* RowData = Row.Value;

        FDropData* DropData = reinterpret_cast<FDropData*>(const_cast<uint8*>(RowData));
        
        if (DropData->CardCode != GetCardID()) continue;

        ACardStack* NewCardStack = nullptr;

        int TotalWeight = 0;
        for (int CardWeight : DropData->DropWeight)
        {
            TotalWeight += CardWeight;
        }

        int LeftSpawnCount = SpawnCount;

        while (LeftSpawnCount > 0)
        {
            int RandomNumber = FMath::RandRange(1, TotalWeight);
            for (int i = 0; i < DropData->DropCardCode.Num(); i++)
            {
                RandomNumber -= DropData->DropWeight[i];
                if (RandomNumber < 1)
                {
                    NewCardStack = SLGameMode->SpawnCard(Location, DropData->DropCardCode[i]);
                    break;
                }
            }
            if (NewCardStack != nullptr)
            {
                NewCardStack->GetFirstCard()->Push();
            }
            LeftSpawnCount--;
        }

        Cast<ACardStack>(CardStack)->RemoveCard(this, true);
    }
}