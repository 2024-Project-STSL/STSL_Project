// Fill out your copyright notice in the Description page of Project Settings.


#include "CardArea.h"
#include "CardStack.h"
#include "SLGameModeBase.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ACardArea::ACardArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardAreaMesh"));
    RootComponent = VisualMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CardVisualAsset(TEXT("/Script/Engine.StaticMesh'/Game/Mesh/CardBaseMesh.CardBaseMesh'"));
    static ConstructorHelpers::FObjectFinder<UMaterial> CardMaterialAsset(TEXT("/Script/Engine.Material'/Game/Material/BlackMaterial.BlackMaterial'"));

    if (CardVisualAsset.Succeeded() && CardMaterialAsset.Succeeded())
    {
        VisualMesh->SetStaticMesh(CardVisualAsset.Object);
        VisualMesh->SetMaterial(0, CardMaterialAsset.Object);
    }

}

// Called when the game starts or when spawned
void ACardArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACardArea::SpawnCoinStack(FVector Location, int Amount)
{
    if (Amount > 0)
    {
        TArray<ACardStack*> NewCoins;

        ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
        Location.X -= OutputOffset;

        for (int i = 0; i < Amount; i++)
        {
            NewCoins.Add(SLGameMode->SpawnCard(Location, CoinID));
        }
        for (int i = 1; i < Amount; i++)
        {
            NewCoins[0]->AddCard(NewCoins[i]->GetFirstCard());
        }
    }
}

// Called every frame
void ACardArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

