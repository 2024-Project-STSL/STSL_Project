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

	// ������ �о ���Ϳ� ���� ���
	UPROPERTY(EditDefaultsOnly)
	float OtherCollsionWeight = -1.0f;

public:	
	// Sets default values for this actor's properties
	ACardStack();

protected:
	USceneComponent* SceneComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// �ڽ��� ��ġ�� ù ī���� ��ġ�� �ݿ��ϰ�
	// ���� ������ ���� ���ݾ� Ʋ������ ī�� ��ġ ����
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

	// ī�� �浹 ����
	void GetCardCollisionVector(AActor* Other, FVector& SelfVector, FVector& OtherVector) const;
	void PushCards(FVector Force);

	// ��� ���ÿ� �� ������ ���� �� �ִ°�?
	static bool IsCardStackable(ACardStack* CardStack, ACardStack* OtherStack);

	// ���� index ������ ������ ������ ���ο� �������� ����
	static void SplitCardStack(ACardStack* CardStack, int32 Index);
};
