// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/ListView.h>
#include <Data/CardData.h>
#include <Components/TextBlock.h>
#include "CardGuideMenuBase.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API UCardGuideMenuBase : public UUserWidget
{
	GENERATED_BODY()
	
	class UDataTable* CardDataTable;

	class UDataTable* CraftingRecipeTable;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardDetail;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UListView> CardListView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardIngrediant1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardIngrediant2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardIngrediant3;

protected:
	virtual void NativeConstruct() override;

	void OnCardClicked(UObject* CardObject);

public:
	UCardGuideMenuBase(const FObjectInitializer& ObjectInitializer);

	UCardEntryData* ConvertToEntryData(FCardData* CardData);

	void ClearCardDetail();

	void ShowCardDetail(int CardCode);
};
