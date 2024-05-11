// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/TextRenderComponent.h>
#include <Components/WidgetComponent.h>
#include "Engine/Font.h"
#include "BuySellInterface.h"
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

	// ī���� �̸�
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* PackNameText;

	// ī���� ����
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* PackPriceText;

	UPROPERTY(EditAnywhere, Category = "Fonts")
	float NameFontSize = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Fonts")
	float PriceFontSize = 80.0f;

	// ī������ ����
	UPROPERTY(EditAnywhere, Category = "Fonts")
	int TotalCardPrice = 999;

	// ���� ������ ���� �����ϰ� ���� ī���� ����
	UPROPERTY(EditAnywhere, Category = "Fonts")
	int CurrentCardPrice = 999;

	void UpdatePriceText();
public:
	// Sets default values for this actor's properties
	ABuyArea();


protected:

public:

	virtual void BuyCard(ACardStack* CardStack) override;

};

