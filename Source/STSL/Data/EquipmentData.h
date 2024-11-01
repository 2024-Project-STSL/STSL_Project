// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "Data/EffectData.h"
#include "EquipmentData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EquipType : uint8
{
	SubArmor = 0,
	MainArmor,
	Weapon
};

UENUM(BlueprintType)
enum class StatType : uint8
{
	Attack = 0,
	Speed,
	Defence
};

USTRUCT(Atomic, BlueprintType)
struct STSL_API FEquipmentData : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CardCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EquipType EquipType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	StatType StatType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int StatValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EffectCode EffectType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EffectRate;

	FEquipmentData();
	~FEquipmentData();
};
