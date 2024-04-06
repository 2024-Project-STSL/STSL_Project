// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/ProgressBar.h>
#include "CraftingProgressBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API UCraftingProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	UProgressBar* CraftingProgressBar;

	UPROPERTY(EditAnywhere)
	float MaxProgress = 1.0f;

	UPROPERTY(EditAnywhere)
	float CurrentProgress = 1.0f;

public:
	void NativeConstruct() override;

	void SetProgress(float Current, float Max);
	void SetProgress(float Current);
};
