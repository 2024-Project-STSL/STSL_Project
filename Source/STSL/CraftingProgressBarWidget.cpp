// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingProgressBarWidget.h"

void UCraftingProgressBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CraftProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("CraftingProgressBar")));
	CraftProgressBar->SetPercent(CurrentProgress / MaxProgress);

}