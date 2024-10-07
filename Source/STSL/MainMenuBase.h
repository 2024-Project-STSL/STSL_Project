// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLGameModeBase.h"
#include "Components/CanvasPanelSlot.h"
#include <Components/Button.h>
#include <Components/TextBlock.h>
#include "MainMenuBase.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API UMainMenuBase : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="Icon")
	TObjectPtr<UTexture2D> PlayIconOn;
	
	UPROPERTY(VisibleAnywhere, Category = "Icon")
	TObjectPtr<UTexture2D> PlayIconOff;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	TObjectPtr<UTexture2D> PauseIconOn;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	TObjectPtr<UTexture2D> PauseIconOff;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush PlayBrushOn;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush PlayBrushOff;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush PauseBrushOn;

	UPROPERTY(VisibleAnywhere, Category = "Icon")
	FSlateBrush PauseBrushOff;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PauseButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CardGuideButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardGuideIndicator;
public:

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UUserWidget> CardGuideMenu;

	UMainMenuBase(const FObjectInitializer& ObjectInitializer);

	void UpdateIcon(GamePlayState GameState);

	UFUNCTION(BlueprintCallable, Category = "CardGuide")
	void OpenCardGuide();

	UFUNCTION(BlueprintCallable, Category = "CardGuide")
	void CloseCardGuide();
};
