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
#include "FieldManager.h"
#include "SLSaveGame.h"
#include "Data/CardData.h"
#include "SLGameStateBase.h"
#include "SLGameModeBase.generated.h"

UCLASS()
class STSL_API ASLGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	TObjectPtr<UUserWidget> MainMenu;
	TObjectPtr<UUserWidget> BreakMenu;
	TObjectPtr<UUserWidget> PauseMenu;
	TObjectPtr<UUserWidget> GameoverMenu;

	TObjectPtr<ASLGameStateBase> SLGameState;

	class UDataTable* CardDataTable;

	UPROPERTY(EditAnywhere, Category = "World")
	TObjectPtr<AFieldManager> FieldManager;

	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TArray<ACard*> People;
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TArray<ACard*> Foods;
	int PersonIndex; int FoodIndex;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> SellAreaActor;

	FTimerHandle AutoSaveHandle;

	UPROPERTY(VisibleAnywhere, Category = "Save")
	float AutoSaveInterval = 60.0f;

protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void StartPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

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
	GamePlayState GetPlayState() const { return GetGameState<ASLGameStateBase>()->CurrentPlayState; }

	UFUNCTION(BlueprintCallable, Category = "Time")
	float GetDayProgressPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveGame() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void ResetGame();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void HandleDeath(ACharacterCard* DeadCard);

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void LoadBattle(const TArray<FVector>& BattleLocation);

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartBattle(ACardStack* FirstStack, ACardStack* SecondStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	int GetTotalCardAmount(bool ExcludeCoin) const;

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	int GetExcessiveCardAmount() const { return GetTotalCardAmount(true) - GetGameState<ASLGameStateBase>()->CardLimit; }

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void AddCardStack(ACardStack* CardStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void RemoveCardStack(ACardStack* CardStack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	TArray<ACardStack*> GetAllCardStacks() const { return GetGameState<ASLGameStateBase>()->CardStacks; }

	UFUNCTION(BlueprintCallable, Category = "World")
	void UpdateCardLimit();

	UFUNCTION(BlueprintCallable, Category = "World")
	AFieldManager* GetFieldManager() const { return FieldManager; };

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void SetCardHighlight(bool bCardHighlight, ACardStack* NewDraggingStack = nullptr);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	ACardStack* GetDraggingStack() const { return SLGameState->DraggingStack; };

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void EmptyDraggingStack() { SLGameState->DraggingStack = nullptr; };

	UFUNCTION(BlueprintCallable, Category = "CardStack", BlueprintPure = false)
	ACardStack* SpawnCard(FVector Location, int CardID);

	UFUNCTION(BlueprintCallable, Category = "CardStack", BlueprintPure = false)
	ACardStack* SpawnCardPack(FVector Location, int PackID);
};
