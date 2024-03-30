// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "EquipmentData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EquipType 
{
	SubArmor = 0,
	MainArmor,
	Weapon
};

UENUM(BlueprintType)
enum class StatType 
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

	FEquipmentData();
	~FEquipmentData();
};
