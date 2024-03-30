// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "RecipeData.generated.h"

/**
 * 
 */

USTRUCT(Atomic, BlueprintType)
struct STSL_API FRecipeData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int CardCode;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TArray<int> ReqCardCode;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TArray<int> ReqValue;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TArray<bool> bIsDeleted;

	FRecipeData();
	~FRecipeData();
};
