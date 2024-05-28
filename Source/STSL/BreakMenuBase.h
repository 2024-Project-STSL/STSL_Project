// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Button.h>
#include "BreakMenuBase.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API UBreakMenuBase : public UUserWidget
{
	GENERATED_BODY()
	
	UButton* EatButton;

public:
	UBreakMenuBase(const FObjectInitializer& ObjectInitializer);
};
