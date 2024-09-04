// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharactorCard.h"
#include "BattleManager.generated.h"

UCLASS()
class STSL_API ABattleManager : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TArray<ACharactorCard*> FirstTeam;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TArray<ACharactorCard*> SecondTeam;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TObjectPtr<ACharactorCard> CurrentAttacker = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TObjectPtr<ACharactorCard> CurrentVictim = nullptr;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ABattleManager();
	ABattleManager(TArray<ACharactorCard*> Team1, TArray<ACharactorCard*> Team2);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Attack(ACharactorCard* Attacker, ACharactorCard* Victim);

	UFUNCTION(BlueprintCallable)
	void SetTeam(TArray<ACharactorCard*> Team1, TArray<ACharactorCard*> Team2);

	UFUNCTION(BlueprintCallable)
	void HandleDeath(ACharactorCard* DeadCard);
};
