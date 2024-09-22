// Fill out your copyright notice in the Description page of Project Settings.

#include "EffectData.h"
#include "CharacterCard.h"

FEffectData::FEffectData()
{
}

FEffectData::~FEffectData()
{
}

void FEffectData::Tick(AActor* TargetCard)
{
	TObjectPtr<ACharacterCard> Character = Cast<ACharacterCard>(TargetCard);

	if (Character == nullptr) return;

	if (LastTick == -1.0f) LastTick = EffectTime;

	switch (EffectCode)
	{
	case EffectCode::Poison:
		if (LastTick - EffectTime > 2.0f)
		{
			Character->CharacterDamage(1);
			LastTick = EffectTime;
			break;
		}
	default:
		break;
	}
}
