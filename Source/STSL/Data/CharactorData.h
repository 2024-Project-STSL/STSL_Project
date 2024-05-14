// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "CharactorData.generated.h"

/**
 * 
 */

USTRUCT(Atomic, BlueprintType)
struct STSL_API FCharactorData : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CardCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CharHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CharSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CharAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CharDefence;

	FCharactorData();
	~FCharactorData();
};