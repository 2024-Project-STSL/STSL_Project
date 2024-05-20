// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLGameModeBase.h"
#include <Components/Button.h>
#include "MainMenuBase.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API UMainMenuBase : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="Icon")
	UTexture2D* PlayIconOn;
	
	UPROPERTY(VisibleAnywhere, Category = "Icon")
	UTexture2D* PlayIconOff;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	UTexture2D* PauseIconOn;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	UTexture2D* PauseIconOff;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush PlayBrushOn;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush PlayBrushOff;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush PauseBrushOn;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush PauseBrushOff;

	UButton* PlayButton;
	UButton* PauseButton;

public:
	UMainMenuBase(const FObjectInitializer& ObjectInitializer);

	void UpdateIcon(GamePlayState GameState);
};
