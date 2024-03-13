// Fill out your copyright notice in the Description page of Project Settings.

#include "Card.h"
#include "CardStack.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ACard::ACard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    VisualMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CardVisualAsset(TEXT("/Script/Engine.StaticMesh'/Game/Mesh/CardMesh.CardMesh'"));

    if (CardVisualAsset.Succeeded())
    {
        VisualMesh->SetStaticMesh(CardVisualAsset.Object);
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        VisualMesh->SetSimulatePhysics(true);
        VisualMesh->SetRelativeScale3D(FVector(3.2f, 2.0f, 0.05f));

        VisualMesh->BodyInstance.bLockXRotation = true;
        VisualMesh->BodyInstance.bLockYRotation = true;
        VisualMesh->BodyInstance.bLockZRotation = true;
        //VisualMesh->BodyInstance.SetDOFLock(EDOFMode::SixDOF);
    }
}

ACard::ACard(uint32 CardID)
{
    ACard::ACard();
    this->CardID = CardID;
}

// Called when the game starts or when spawned
void ACard::BeginPlay()
{
	Super::BeginPlay();
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
    CardStackActor->HandleStackMove(Movement);
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
    FVector ActorLocation = GetActorLocation();
    FVector HitLocation = GetMouseHitLocation();
    CardOffset = ActorLocation - HitLocation;
    FString VectorString = FString::Printf(TEXT("X: %f, Y: %f, Z: %f"), CardOffset.X, CardOffset.Y, CardOffset.Z);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, VectorString);
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