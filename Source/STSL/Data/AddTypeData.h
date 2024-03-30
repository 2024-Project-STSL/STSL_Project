// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "AddTypeData.generated.h"

/**
 * 
 */

USTRUCT(Atomic, BlueprintType)
struct STSL_API FAddTypeData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AddType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AddTypeName;

	FAddTypeData();
	~FAddTypeData();
};
