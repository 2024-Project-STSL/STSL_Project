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
	int32 CardID = 0;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VisualMesh;

	UPROPERTY(VisibleAnywhere)
	FVector CardOffset;

	UPROPERTY(EditDefaultsOnly)
	float CardMass = 10.0f;

public:	
	// Sets default values for this actor's properties
	ACard();
	ACard(int32 CardID);

	UFUNCTION(BlueprintCallable, Category = "CardID")
	int32 GetCardID() const { return CardID; }

	UFUNCTION(BlueprintCallable, Category = "CardID")
	void SetCardID(int32 NewCardID) { CardID = NewCardID; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitCompoent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetCardStack(AActor* Stack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	AActor* GetCardStack() const { return CardStack; }

	UStaticMeshComponent* GetVisualMesh() const { return VisualMesh; }

	// 마우스 카드 이동 관련
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

	// 벡터만큼 카드 밀어내기
	void Push(FVector Force);
};
