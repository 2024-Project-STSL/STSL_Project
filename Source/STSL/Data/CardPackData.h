// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "CardPackData.generated.h"

/**
 *
 */

USTRUCT(Atomic, BlueprintType)
struct STSL_API FCardPackData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int PackCode;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString PackName;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int PackValue;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int PackPrice;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TArray<int> CardList;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TArray<int> CardWeight;

	FCardPackData();
	~FCardPackData();
};
