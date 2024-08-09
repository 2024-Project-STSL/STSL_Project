// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card.h"
#include "Data/CardPackData.h"
#include "CardPack.generated.h"

/**
 * 
 */
UCLASS()
class STSL_API ACardPack : public ACard
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FCardPackData CardPackData;

	UMaterialInstance* CardPackMat;

	UDataTable* CardPackDataTable;
	
	// 드래그를 시작한 포지션
	FVector2D StartDragPos;

	// 과 드래그 끝 포지션이 이보다 작으면 카드팩 개봉
	float OpenThreshold = 30.0f;

	// 카드팩 개봉 후 일정 시간 공중에 유지
	UPROPERTY(EditAnyWhere)
	float MaxFloatingTime = 1.0f;

	UPROPERTY(VisibleAnyWhere)
	float FloatingTime = 0.0f;

protected:
	void LoadCard() override;
	void Tick(float DeltaTime) override;

public:
	ACardPack();
	ACardPack(int32 PackID);

	void SetCardID(int32 NewCardID) override;

	void StartCardDrag() override;
	void EndCardDrag() override;

	void SpawnCard();
};
