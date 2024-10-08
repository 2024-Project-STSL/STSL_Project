// Fill out your copyright notice in the Description page of Project Settings.


#include "CardListEntryBase.h"

void UCardListEntryBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	TObjectPtr<UCardEntryData> CardEntryData = Cast<UCardEntryData>(ListItemObject);

	if (CardEntryData != nullptr)
	{
		CardEntryName->SetText(FText::FromString(CardEntryData->CardName));
	}
}
