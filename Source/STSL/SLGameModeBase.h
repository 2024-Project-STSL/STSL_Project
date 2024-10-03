// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CardStack.h"
#include "Card.h"
#include "CharacterCard.h"
#include "PersonCard.h"
#include "PortalCard.h"
#include "CardPack.h"
#include "MainActor.h"
#include "BattleManager.h"
#include "Data/CardData.h"
#include "SLGameModeBase.generated.h"

UENUM(BluePrintType)
enum class GamePlayState : uint8 
{
	PlayState = 0,
	BreakState,
	PauseState,
	GameoverState
};

UCLASS()
class STSL_API ASLGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	TObjectPtr<UUserWidget> MainMenu;
	TObjectPtr<UUserWidget> BreakMenu;
	TObjectPtr<UUserWidget> PauseMenu;
	TObjectPtr<UUserWidget> GameoverMenu;

	class UDataTable* CardDataTable;

	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TArray<ACardStack*> CardStacks;

	// 가장 최근 마우스에 의해 드래그 시작된 스택
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TObjectPtr<ACardStack> DraggingStack;

	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	bool bIsCardHighlight;

	UPROPERTY(VisibleAnywhere, Category = "World")
	TMap<FString, float> BaseWorldBorder;

	UPROPERTY(VisibleAnywhere, Category = "World")
	TMap<FString, float> WorldBorder;

	UPROPERTY(EditAnywhere, Category = "World")
	FVector BaseFloorScale = FVector(4.0f, 6.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "World")
	float ExtendRange = 500.0f;

	UPROPERTY(EditAnywhere, Category = "World")
	float WallOffset = 50.0f;

	UPROPERTY(EditAnywhere, Category = "World")
	float BuyAreaHeight = 800.0f;

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

	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TArray<ACard*> People;
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TArray<ACard*> Foods;
	int PersonIndex; int FoodIndex;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> SellAreaActor;

protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void CreateMenu();

	UFUNCTION()
	void OnSellCardHandler();

public:
	ASLGameModeBase();

	UFUNCTION(BlueprintCallable, Category = "Time")
	void PauseGame(bool bForce=false);

	UFUNCTION(BlueprintCallable, Category = "Time")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, Category = "Time")
	void BreakGame();

	UFUNCTION(BlueprintCallable, Category = "Time")
	void Gameover();

	UFUNCTION(BlueprintCallable, Category = "TurnEnd")
	void Eat();
	UFUNCTION(BlueprintCallable, Category = "TurnEnd")
	void EatNext();
	UFUNCTION(BlueprintCallable, Category = "TurnEnd")
	void EatCompleted();

	UFUNCTION(BlueprintCallable, Category = "TurnEnd")
	void MoveBackCompleted();

	UFUNCTION(BlueprintCallable, Category = "TurnEnd")
	void CheckHunger();

	UFUNCTION(BlueprintCallable, Category = "TurnEnd")
	bool CheckGameover();

	UFUNCTION(BlueprintCallable, Category = "TurnEnd")
	void CheckExcessiveCards();

	UFUNCTION(BlueprintCallable, Category = "TurnEnd")
	void CheckDayEnd();
	UFUNCTION(BlueprintCallable, Category = "TurnEnd")
	void EndDay();

	UFUNCTION(BlueprintCallable, Category = "Time")
	int GetPortalSpawnCount() const;

	UFUNCTION(BlueprintCallable, Category = "Time")
	GamePlayState GetPlayState() const { return CurrentPlayState; }

	UFUNCTION(BlueprintCallable, Category = "Time")
	float GetDayProgressPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void HandleDeath(ACharacterCard* DeadCard);

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartBattle(ACardStack* FirstStack, ACardStack* SecondStack) const;

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	int GetTotalCardAmount(bool ExcludeCoin) const;

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	int GetExcessiveCardAmount() const { return GetTotalCardAmount(true) - CardLimit; }

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void AddCardStack(ACardStack* CardStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void RemoveCardStack(ACardStack* CardStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	TArray<ACardStack*> GetAllCardStacks() const { return CardStacks; }

	UFUNCTION(BlueprintCallable, Category = "World")
	void UpdateCardLimit();

	UFUNCTION(BlueprintCallable, Category = "World")
	TMap<FString, float> GetWorldBorder(bool bExcludeBuyArea = false) const;

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void SetCardHighlight(bool bCardHighlight, ACardStack* NewDraggingStack = nullptr);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	ACardStack* GetDraggingStack() const { return DraggingStack; };

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void EmptyDraggingStack() { DraggingStack = nullptr; };

	UFUNCTION(BlueprintCallable, Category = "CardStack", BlueprintPure = false)
	ACardStack* SpawnCard(FVector Location, int CardID);

	UFUNCTION(BlueprintCallable, Category = "CardStack", BlueprintPure = false)
	ACardStack* SpawnCardPack(FVector Location, int PackID);
};
