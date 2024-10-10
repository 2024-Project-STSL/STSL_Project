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

	// 가장 최근 마우스에 의해 드래그 시작된 스택
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

	// 각 일차에 주어진 시간
	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeForDay = 120.0f;

	// 현재 경과한 시간
	UPROPERTY(EditAnywhere, Category = "Time")
	float Time = 0.0f;

	// 초과 카드 정산 중인가, 즉 플레이어가 임의로 시간을 진행시킬 수 있는가?
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
