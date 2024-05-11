// Fill out your copyright notice in the Description page of Project Settings.


#include "CardArea.h"

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

// Called every frame
void ACardArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

