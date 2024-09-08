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
	TObjectPtr<UStaticMeshComponent> BattleCube;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TArray<ACharacterCard*> FirstTeam;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TArray<ACharacterCard*> SecondTeam;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TObjectPtr<ACharacterCard> CurrentAttacker = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Battle")
	TObjectPtr<ACharacterCard> CurrentVictim = nullptr;

	UPROPERTY(EditAnywhere, Category = "Battle")
	float BattleCubeWidthPerCard = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Battle")
	float BattleCubeYOffset = 450.0f;

	UPROPERTY(EditAnywhere, Category = "Battle")
	float BattleCubeBaseWidth = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Battle")
	float BattleCubeBaseHeight = 1800.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ACharacterCard* GetAttacker(TArray<ACharacterCard*> Candidates);

	ACharacterCard* GetVictim(ACharacterCard* Attacker);

	void Relocate();

	void DamageVictim();

	void MovebackCompleted();

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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
	void JoinBattle(ACharacterCard* TargetCard);

	void LeaveBattle(ACharacterCard* TargetCard);

	UFUNCTION(BlueprintCallable)
	void HandleDeath(ACharacterCard* DeadCard);

	UFUNCTION(BlueprintCallable)
	void EndBattle();
};
