// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CardStack.h>
#include <FieldManager.h>
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SLGameStateBase.generated.h"

/**
 * 
 */

UENUM(BluePrintType)
enum class GamePlayState : uint8
{
	PlayState = 0,
	BreakState,
	PauseState,
	GameoverState
};

UCLASS()
class STSL_API ASLGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TArray<ACardStack*> CardStacks;

	// ���� �ֱ� ���콺�� ���� �巡�� ���۵� ����
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TObjectPtr<ACardStack> DraggingStack;

	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	bool bIsCardHighlight;

	UPROPERTY(EditAnywhere, Category = "World")
	int BaseCardLimit = 30;

	UPROPERTY(EditAnywhere, Category = "World")
	int CardLimit = 30;

	UPROPERTY(EditAnywhere, Category = "Time")
	GamePlayState CurrentPlayState = GamePlayState::PlayState;

	UPROPERTY(EditAnywhere, Category = "Time")
	int Day = 1;

	// �� ������ �־��� �ð�
	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeForDay = 120.0f;

	// ���� ����� �ð�
	UPROPERTY(EditAnywhere, Category = "Time")
	float Time = 0.0f;

	// �ʰ� ī�� ���� ���ΰ�, �� �÷��̾ ���Ƿ� �ð��� �����ų �� �ִ°�?
	UPROPERTY(VisibleAnywhere, Category = "Time")
	bool bSellingExcessiveCard = false;

	UPROPERTY(EditAnywhere, Category = "Portal")
	int PortalStartingDay = 12;

	UPROPERTY(EditAnywhere, Category = "Portal")
	int PortalIntervalDay = 6;

	UPROPERTY(EditAnywhere, Category = "Portal")
	int InitSpawnCount = 2;

	UPROPERTY(EditAnywhere, Category = "Portal")
	int MaxSpawnCount = 5;

};
