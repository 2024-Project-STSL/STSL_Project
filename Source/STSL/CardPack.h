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
	
	// �巡�׸� ������ ������
	FVector2D StartDragPos;

	// �� �巡�� �� �������� �̺��� ������ ī���� ����
	float OpenThreshold = 30.0f;

	// ī���� ���� �� ���� �ð� ���߿� ����
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
