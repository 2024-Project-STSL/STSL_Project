// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include <SLGameStateBase.h>
#include <CharacterCard.h>
#include <Data/EffectData.h>
#include <Data/EquipmentData.h>
#include "SLSaveGame.generated.h"

USTRUCT(Atomic, BlueprintType)
struct STSL_API FCharacterSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FCharacterData CharacterData;

	UPROPERTY()
	float CurrentDropCooldown;

	UPROPERTY()
	EBattleState BattleState;

	UPROPERTY()
	float AttackGauge;

	UPROPERTY()
	float MaxAttackGauge;

	UPROPERTY()
	TArray<FEffectData> AppliedEffects;

	void SaveData(AActor* TargetCard);

	void LoadData(AActor* TargetCard) const;
	
};

USTRUCT(Atomic, BlueprintType)
struct STSL_API FCardStackSaveData
{
	GENERATED_BODY()

public:
	int LoadedAmount = 0;
	int LoadedCharAmount = 0;
	int LoadedPackAmount = 0;
	int LoadedEquipAmount = 0;

	UPROPERTY()
	float CraftingProgress;

	UPROPERTY()
	FVector CardLocation;

	UPROPERTY()
	TArray<FCharacterSaveData> CharacterData;

	UPROPERTY()
	TArray<int32> CardPackValue;

	UPROPERTY()
	TArray<FEquipmentData> EquipmentData;

	UPROPERTY()
	TArray<FCardData> CardData;

	void AddCard(ACard* TargetCard);

	// 해당 카드에 데이터 입력
	void LoadCard(ACard* TargetCard);

	int GetNextCardCode() const { return CardData[LoadedAmount].CardCode; }

	bool IsEmpty() const { return LoadedAmount == CardData.Num(); }
};

/**
 * 
 */
UCLASS()
class STSL_API USLSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	USLSaveGame();

	UPROPERTY()
	int32 Version = 1;

	UPROPERTY()
	float MasterVolume;

	UPROPERTY()
	float BackgroundVolume;

	UPROPERTY()
	float EffectVolume;

	UPROPERTY(BlueprintReadWrite, Category = "GameState")
	int32 Day;

	UPROPERTY()
	int32 Time;

	UPROPERTY()
	int32 CardLimit;

	UPROPERTY()
	GamePlayState CurrentPlayState;

	UPROPERTY()
	TMap<FString, int> CurrentBuyAreaPrice;

	UPROPERTY()
	TArray<FCardStackSaveData> AllCardStacks;
};
