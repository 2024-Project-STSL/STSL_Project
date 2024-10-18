// Fill out your copyright notice in the Description page of Project Settings.


#include "SLSaveGame.h"
#include <PersonCard.h>
#include <CardPack.h>

void FCharacterSaveData::SaveData(AActor* TargetActor)
{
	ACharacterCard* TargetCard = Cast<ACharacterCard>(TargetActor);

	CharacterData = TargetCard->GetCharacterStat();

	CurrentDropCooldown = TargetCard->CurrentDropCooldown;

	BattleState = TargetCard->BattleState;

	AttackGauge = TargetCard->AttackGauge;

	MaxAttackGauge = TargetCard->MaxAttackGauge;

	AppliedEffects = TargetCard->AppliedEffects;
	
}

void FCharacterSaveData::LoadData(AActor* TargetActor) const
{
	ACharacterCard* TargetCard = Cast<ACharacterCard>(TargetActor);

	TargetCard->GetCharacterStat() = CharacterData;

	TargetCard->CurrentDropCooldown = CurrentDropCooldown;

	TargetCard->BattleState = BattleState;

	TargetCard->AttackGauge = AttackGauge;

	TargetCard->MaxAttackGauge = MaxAttackGauge;

	TargetCard->AppliedEffects = AppliedEffects;
	
}

void FCardStackSaveData::AddCard(ACard* TargetCard)
{
	FCardData TargetData;
	TargetData.AddTypeValue = TargetCard->GetAddTypeValue();
	TargetData.CardCode = TargetCard->GetCardID();
	TargetData.CardType = TargetCard->GetCardType();

	CardData.Add(TargetData);

	if (TargetData.CardType == CardType::pack)
	{
		CardPackValue.Add(Cast<ACardPack>(TargetCard)->CardPackData.PackValue);
	}
	else if (TargetData.IsCharacter())
	{
		FCharacterSaveData TargetCharData;
		TargetCharData.SaveData(Cast<ACharacterCard>(TargetCard));

		CharacterData.Add(TargetCharData);

		if (TargetData.CardType == CardType::person)
		{
			FEquipmentData TargetEquipData;
			TargetEquipData = Cast<APersonCard>(TargetCard)->GetEquipment(EquipType::Weapon);
			EquipmentData.Add(TargetEquipData);
			TargetEquipData = Cast<APersonCard>(TargetCard)->GetEquipment(EquipType::MainArmor);
			EquipmentData.Add(TargetEquipData);
			TargetEquipData = Cast<APersonCard>(TargetCard)->GetEquipment(EquipType::SubArmor);
			EquipmentData.Add(TargetEquipData);
		}
	}
}

void FCardStackSaveData::LoadCard(ACard* TargetCard)
{
	FCardData TargetData = CardData[LoadedAmount];

	TargetCard->SetCardID(TargetData.CardCode);
	TargetCard->SetAddTypeValue(TargetData.AddTypeValue);
	
	if (TargetData.CardType == CardType::pack)
	{
		Cast<ACardPack>(TargetCard)->CardPackData.PackValue = CardPackValue[LoadedPackAmount];
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::FromInt(CardPackValue[LoadedPackAmount]));
		LoadedPackAmount++;
	}
	else if (TargetData.IsCharacter())
	{
		FCharacterSaveData TargetCharData = CharacterData[LoadedCharAmount];
		TargetCharData.LoadData(Cast<ACharacterCard>(TargetCard));

		if (TargetData.CardType == CardType::person)
		{
			FEquipmentData TargetEquipData = EquipmentData[LoadedEquipAmount];
			Cast<APersonCard>(TargetCard)->Equip(TargetEquipData.CardCode, false);
			LoadedEquipAmount++;

			TargetEquipData = EquipmentData[LoadedEquipAmount];
			Cast<APersonCard>(TargetCard)->Equip(TargetEquipData.CardCode, false);
			LoadedEquipAmount++;

			TargetEquipData = EquipmentData[LoadedEquipAmount];
			Cast<APersonCard>(TargetCard)->Equip(TargetEquipData.CardCode, false);
			LoadedEquipAmount++;
		}

		LoadedCharAmount++;
	}

	LoadedAmount++;
}

USLSaveGame::USLSaveGame()
{
}
