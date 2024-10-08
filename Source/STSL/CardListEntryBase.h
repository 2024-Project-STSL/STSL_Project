// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/CardData.h"
#include <Components/ListView.h>
#include <Components/TextBlock.h>
#include "CardListEntryBase.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API UCardListEntryBase : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardEntryName;

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
