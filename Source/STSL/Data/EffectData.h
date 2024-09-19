// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "EffectData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct STSL_API FEffectData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int EffectCode;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString EffectName;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString EffectScript;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float EffectTime;

	FEffectData();
	~FEffectData();
};
