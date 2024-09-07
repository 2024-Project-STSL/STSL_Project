// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterCard.h"
#include "BattleManager.generated.h"

UCLASS()
class STSL_API ABattleManager : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TArray<ACharacterCard*> FirstTeam;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TArray<ACharacterCard*> SecondTeam;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TObjectPtr<ACharacterCard> CurrentAttacker = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TObjectPtr<ACharacterCard> CurrentVictim = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetAttacker(TArray<ACharacterCard*> Candidates);

public:	
	// Sets default values for this actor's properties
	ABattleManager();
	ABattleManager(TArray<ACharacterCard*> Team1, TArray<ACharacterCard*> Team2);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Attack(ACharacterCard* Attacker, ACharacterCard* Victim);

	UFUNCTION(BlueprintCallable)
	void SetTeam(TArray<ACharacterCard*> Team1, TArray<ACharacterCard*> Team2);

	UFUNCTION(BlueprintCallable)
	void HandleDeath(ACharacterCard* DeadCard);
};
