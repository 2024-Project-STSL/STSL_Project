// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Card.h"
#include "MouseInputInterface.h"
#include <Components/BoxComponent.h>
#include "CardStack.generated.h"

UCLASS()
class STSL_API ACardStack : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "CardStack")
	TArray<AActor*> Cards;

	// X Offset between cards
	UPROPERTY(EditDefaultsOnly, Category = "CardStack")
	float XOffset = -50.0f;

	// Z Offset between cards
	UPROPERTY(EditDefaultsOnly, Category = "CardStack")
	float ZOffset = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float HoveringHeight = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float FloatingHeight = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float HeightOffset = 0.2f;

	UPROPERTY(EditDefaultsOnly)
	float Collsionforce = 50.0f;

	// 상대방을 밀어낼 벡터에 곱할 계수
	UPROPERTY(EditDefaultsOnly)
	float OtherCollsionWeight = -1.0f;

public:	
	// Sets default values for this actor's properties
	ACardStack();

protected:
	USceneComponent* SceneComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 자신의 위치를 첫 카드의 위치로 반영하고
	// 물리 엔진에 의해 조금씩 틀어지는 카드 위치 보정
	void UpdatePosition();

	void RemoveFromGamemode();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Add a card to the stack
	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void AddCard(AActor* Card);

	void RemoveCard(AActor* Card, bool bDespawn = false);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void RemoveCard(int32 Index, bool bDespawn = false);

	ACard* GetLastCard() const { return Cast<ACard>(Cards.Last()); }

	void HandleStackMove(ACard* Sender, ECardMovement Movement);

	int32 FindMouseSender() const;

	void HandleStackCollision(ACard* OtherCard);

	// 카드 충돌 관련
	void GetCardCollisionVector(AActor* Other, FVector& SelfVector, FVector& OtherVector) const;
	void PushCards(FVector Force);

	// 상대 스택에 이 스택이 쌓일 수 있는가?
	static bool IsCardStackable(ACardStack* CardStack, ACardStack* OtherStack);

	// 기준 index 이전의 스택을 나누어 새로운 스택으로 만듦
	static void SplitCardStack(ACardStack* CardStack, int32 Index);
};
