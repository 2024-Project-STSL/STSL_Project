// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "DropData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct STSL_API FDropData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int CardCode;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TArray<int> DropCardCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> DropWeight;

	FDropData();
	~FDropData();
};
