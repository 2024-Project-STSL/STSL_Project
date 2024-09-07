// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "CardData.generated.h"

UENUM(BlueprintType)
enum class CardType {
	normal = 0,
	resource,
	coin,
	food,
	equip,
	person,
	enemy,
	netural,
	extendment,
	production,
	pack,
	portal
};

UENUM(BlueprintType)
enum class AddType {
	nope = 0,
	dropable,
	foodable
};

USTRUCT(Atomic, BlueprintType)
struct STSL_API FCardData : public FTableRowBase 
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CardCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CardName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	CardType CardType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CardPrice;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AddType AddType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AddTypeValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CardScript;

	FCardData();
	~FCardData();

	bool IsCharacter() const;
};
