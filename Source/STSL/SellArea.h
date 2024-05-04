// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TextRenderComponent.h>
#include <Components/WidgetComponent.h>
#include "Engine/Font.h"
#include "BuySellInterface.h"
#include "SellArea.generated.h"

UCLASS()
class STSL_API ASellArea : public AActor, public IBuySellInterface
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* VisualMesh;

	UFont* CardFont;
	UMaterial* CardFontMat;

	// 카드 제목 텍스트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* SellText;

	UPROPERTY(EditAnywhere, Category = "Fonts")
	float FontSize = 150.0f;

public:	
	// Sets default values for this actor's properties
	ASellArea();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SellCard(ACardStack* CardStack) override;

};
