// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"

// Sets default values
ABattleManager::ABattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ABattleManager::ABattleManager(TArray<ACharacterCard*> Team1, TArray<ACharacterCard*> Team2)
{
	ABattleManager();
	FirstTeam = Team1;
	SecondTeam = Team2;
}

// Called when the game starts or when spawned
void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	while (FirstTeam.Num() != 0 && SecondTeam.Num() != 0)
	{
		Attack(FirstTeam[0], SecondTeam[0]);
		if (FirstTeam.Num() == 0 || SecondTeam.Num() == 0) break;
		Attack(SecondTeam[0], FirstTeam[0]);
	}
	if (FirstTeam.Num() == 0 || SecondTeam.Num() == 0) Destroy();
}

void ABattleManager::Attack(ACharacterCard* Attacker, ACharacterCard* Victim)
{
	CurrentAttacker = Attacker;
	CurrentVictim = Victim;

	FCharacterData AttackerStat = Attacker->GetCharacterStat();
	FCharacterData VictimStat = Victim->GetCharacterStat();

	bool Crit = FMath::RandRange(0.0f, 1.0f) < AttackerStat.CharCritRate;
	float FinalDamage = 0.0f;
	float DamageMulti = 0.0f;

	if (Crit)
	{
		DamageMulti = 1.5f;
	}
	else {
		DamageMulti = FMath::RandRange(0.8f, 1.0f);
	}

	FinalDamage = FMath::Max(0.0f, AttackerStat.CharAttack * DamageMulti - VictimStat.CharDefence);
	Victim->CharacterDamage(FMath::CeilToInt(FinalDamage));

	CurrentAttacker = nullptr;
	CurrentVictim = nullptr;
}

void ABattleManager::HandleDeath(ACharacterCard* DeadCard)
{
	FirstTeam.Remove(DeadCard);
	SecondTeam.Remove(DeadCard);
}

void ABattleManager::SetTeam(TArray<ACharacterCard*> Team1, TArray<ACharacterCard*> Team2)
{
	FirstTeam = Team1;
	SecondTeam = Team2;

	for (TObjectPtr<ACharacterCard> FirstChar : FirstTeam)
	{
		FirstChar->SetBattleState(EBattleState::Wait);
		FirstChar->OnDeath.AddDynamic(this, &ABattleManager::HandleDeath);
	}

	for (TObjectPtr<ACharacterCard> SecondChar : SecondTeam)
	{
		SecondChar->SetBattleState(EBattleState::Wait);
		SecondChar->OnDeath.AddDynamic(this, &ABattleManager::HandleDeath);
	}
}

