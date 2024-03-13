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
    // static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> CardPhysicalMeterial(TEXT("/Script/PhysicsCore.PhysicalMaterial'/Game/Material/CardPhysicalMaterial1.CardPhysicalMaterial1'"));

    if (CardVisualAsset.Succeeded())
    {
        VisualMesh->SetStaticMesh(CardVisualAsset.Object);
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        VisualMesh->SetSimulatePhysics(true);
        VisualMesh->SetRelativeScale3D(FVector(3.2f, 2.0f, 0.05f));
        VisualMesh->SetNotifyRigidBodyCollision(true);
        VisualMesh->SetMassOverrideInKg(NAME_None, CardMass, true);
        //VisualMesh->SetPhysMaterialOverride(CardPhysicalMeterial.Object);
        VisualMesh->BodyInstance.bLockXRotation = true;
        VisualMesh->BodyInstance.bLockYRotation = true;
        VisualMesh->BodyInstance.bLockZRotation = true;
        //VisualMesh->BodyInstance.SetDOFLock(EDOFMode::SixDOF);
    }
}

ACard::ACard(int32 CardID)
{
    ACard::ACard();
    this->CardID = CardID;
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

void ACard::GetCardCollisionVector(AActor* Other, FVector& SelfVector, FVector& OtherVector) const
{
    FVector ActorLocation = GetActorLocation();
    FVector OtherLocation = Other->GetActorLocation();
    FVector CollisionVector = (ActorLocation - OtherLocation).GetSafeNormal(0.0001f);
    CollisionVector.Z = 0.0f;
    CollisionVector *= Collsionforce;
    SelfVector = CollisionVector;
    CollisionVector *= OtherCollsionWeight;
    OtherVector = CollisionVector;
}

void ACard::OnHit(UPrimitiveComponent* HitCompoent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor->IsA<ACard>())
    {
        ACard* OtherCard = Cast<ACard>(OtherActor);
        if (CardID == OtherCard->CardID)
            // 스택
        {
            // TODO
        }
            // 충돌
            // TODO: 충돌도 이동처럼 스택에 알려서 스택이 다 같이 처리해야 함
        else {
            FVector SelfVector, OtherVector;
            GetCardCollisionVector(OtherActor, SelfVector, OtherVector);
            if (VisualMesh->IsSimulatingPhysics())
            {
                // FString VectorString = FString::Printf(TEXT("Other Hit %f %f %f"), SelfVector.X, SelfVector.Y, SelfVector.Z);
                // GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, VectorString);
                VisualMesh->AddForce(SelfVector);
            }
            if (OtherCard->VisualMesh->IsSimulatingPhysics())
            {
                OtherCard->VisualMesh->AddForce(OtherVector);
            }
        }
    }
}