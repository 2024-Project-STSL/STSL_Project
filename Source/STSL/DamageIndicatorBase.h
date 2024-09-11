// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/TextBlock.h>
#include "DamageIndicatorBase.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API UDamageIndicatorBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;

public:

	void SetDamageText(int NewDamage);
};
