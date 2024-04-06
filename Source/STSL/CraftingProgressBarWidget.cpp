// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingProgressBarWidget.h"

void UCraftingProgressBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CraftingProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("CraftingProgressBar")));
	CraftingProgressBar->SetPercent(CurrentProgress / MaxProgress);

}

void UCraftingProgressBarWidget::SetProgress(float Current, float Max)
{
	MaxProgress = Max;
	SetProgress(Current);
	
}

void UCraftingProgressBarWidget::SetProgress(float Current)
{
	CurrentProgress = Current;
	if (CraftingProgressBar != nullptr)
	{
		CraftingProgressBar->SetPercent(CurrentProgress / MaxProgress);
	}
}