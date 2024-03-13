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

public:	
	// Sets default values for this actor's properties
	ACardStack();

protected:
	USceneComponent* SceneComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	AActor* InitCard(AActor* Card, AActor* Other);

	// Add a card to the stack
	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void AddCard(AActor* Card);

	void HandleStackMove(ECardMovement Movement);
};
