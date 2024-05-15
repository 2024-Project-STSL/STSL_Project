// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TextRenderComponent.h>
#include <Components/WidgetComponent.h>
#include "Engine/Font.h"
#include "BuySellInterface.h"
#include "CardArea.generated.h"

UCLASS()
class STSL_API ACardArea : public AActor
{
	GENERATED_BODY()
	
	// 코인 카드의 ID
	UPROPERTY(EditAnywhere)
	int CoinID = 6;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* VisualMesh;

	UPROPERTY(EditAnywhere, Category = "Cards")
	float OutputOffset = 1000.0f;


public:	
	// Sets default values for this actor's properties
	ACardArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnCoinStack(FVector Location, int Amount);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
