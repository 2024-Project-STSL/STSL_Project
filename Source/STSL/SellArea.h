// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TextRenderComponent.h>
#include <Components/WidgetComponent.h>
#include "Engine/Font.h"
#include "BuySellInterface.h"
#include "CardArea.h"
#include "SellArea.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSellCard);

UCLASS()
class STSL_API ASellArea : public ACardArea, public IBuySellInterface
{
	GENERATED_BODY()

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

public:	

	virtual void SellCard(ACardStack* CardStack) override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSellCard OnSellCard;
};
