// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/TextRenderComponent.h>
#include <Components/WidgetComponent.h>
#include "Engine/Font.h"
#include "BuySellInterface.h"
#include "Data/CardPackData.h"
#include "CardArea.h"
#include "BuyArea.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API ABuyArea : public ACardArea, public IBuySellInterface
{
	GENERATED_BODY()
	
	UFont* CardFont;
	UMaterial* CardFontMat;

	UPROPERTY(EditAnywhere)
	FCardPackData CardPackData;

	UDataTable* CardPackDataTable;

	// 카드팩 이름
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* PackNameText;

	// 카드팩 가격
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* PackPriceText;

	UPROPERTY(EditAnywhere, Category = "Fonts")
	float NameFontSize = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Fonts")
	float PriceFontSize = 80.0f;

	// 현재 지불한 양을 제외하고 남은 카드팩 가격
	UPROPERTY(EditAnywhere, Category = "Fonts")
	int CurrentCardPrice = 999;

	void LoadArea();
	void UpdatePriceText();

public:
	// Sets default values for this actor's properties
	ABuyArea();

protected:
	void BeginPlay() override;

public:

	virtual void BuyCard(ACardStack* CardStack) override;

};

