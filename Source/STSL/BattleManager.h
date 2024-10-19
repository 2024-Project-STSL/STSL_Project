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
	TObjectPtr<UStaticMeshComponent> HighlightCube;

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
	float BattleCubeXOffset = 450.0f;

	UPROPERTY(EditAnywhere, Category = "Battle")
	float BattleCubeBaseWidth = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Battle")
	float BattleCubeBaseHeight = 1800.0f;

	UPROPERTY(EditAnywhere, Category = "Battle")
	float AttackMoveDistance = 50.0f;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* AttackArrow;

	UPROPERTY(EditAnywhere, Category = "Battle")
	FVector DamageIndicatorOffset = FVector(50.0f, -50.0f, 10.0f);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* DamageIndicator;

	UPROPERTY(EditAnywhere, Category = "Battle")
	bool bTeamResetted = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ACharacterCard* GetAttacker(TArray<ACharacterCard*> Candidates);

	ACharacterCard* GetVictim(ACharacterCard* Attacker);

	FVector GetCardPosition(int TeamIndex, int CardIndex) const;
	FVector GetCardPosition(ACharacterCard* TargetCard) const;

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
	void SetTeam(TArray<ACharacterCard*>& Team1, TArray<ACharacterCard*>& Team2);

	UFUNCTION(BlueprintCallable)
	void JoinBattle(ACharacterCard* TargetCard);

	UFUNCTION()
	void LeaveBattle(ACharacterCard* TargetCard);

	UFUNCTION(BlueprintCallable)
	void HandleDeath(ACharacterCard* DeadCard);

	UFUNCTION(BlueprintCallable)
	void EndBattle();

	UFUNCTION(BlueprintCallable)
	static void MergeBattle(ABattleManager* Battle1, ABattleManager* Battle2);
};
