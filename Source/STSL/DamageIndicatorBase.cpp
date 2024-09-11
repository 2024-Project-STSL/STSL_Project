// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorBase.h"

void UDamageIndicatorBase::SetDamageText(int NewDamage)
{
	DamageText->SetText(FText::FromString(FString::FromInt(NewDamage)));
}
