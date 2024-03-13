// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MouseInputInterface.h"
#include "Card.generated.h"

UENUM(BlueprintType)
enum class ECardMovement : uint8
{
	StartHover = 0,
	EndHover,
	StartDrag,
	EndDrag,
	MoveToCursor
};

UCLASS()
class STSL_API ACard : public AActor, public IMouseInputInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	AActor* CardStack;

	UPROPERTY(VisibleAnywhere)
	uint32 CardID;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VisualMesh;

	UPROPERTY(VisibleAnywhere)
	FVector CardOffset;

public:	
	// Sets default values for this actor's properties
	ACard();
	ACard(uint32 CardID);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetCardStack(AActor* Stack);
	void SendMovementToStack(ECardMovement Movement);
	FVector GetMouseHitLocation() const;

	virtual void StartMouseHover() override;
	void StartHover(float HoveringHeight);

	virtual void EndMouseHover() override;
	void EndHover();

	virtual void StartDrag() override;
	void StartCardDrag();

	virtual void EndDrag() override;
	void EndCardDrag();

	virtual void MoveToCursor() override;
	void MoveCardToCursor(float FloatingHeight);
};
