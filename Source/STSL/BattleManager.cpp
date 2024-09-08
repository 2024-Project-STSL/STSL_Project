// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include <Kismet/GameplayStatics.h>
#include "SLGameModeBase.h"

// Sets default values
ABattleManager::ABattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BattleCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = BattleCube;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BattleCubeMesh(TEXT("/Script/Engine.StaticMesh'/Game/Mesh/BattleCube.BattleCube'"));

	if (BattleCubeMesh.Succeeded())
	{
		BattleCube->SetStaticMesh(BattleCubeMesh.Object);
		BattleCube->SetSimulatePhysics(false);
		BattleCube->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		BattleCube->SetCollisionProfileName("OverlapAll");

		static ConstructorHelpers::FObjectFinder<UMaterial> BattleCubeMaterial(TEXT("/Script/Engine.Material'/Game/Material/BattleCubeMaterial.BattleCubeMaterial'"));

		if (BattleCubeMaterial.Succeeded())
		{
			BattleCube->SetMaterial(0, BattleCubeMaterial.Object);
		}
	}
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
	BattleCube->OnComponentBeginOverlap.AddDynamic(this, &ABattleManager::OnOverlapBegin);
}

ACharacterCard* ABattleManager::GetAttacker(TArray<ACharacterCard*> Candidates)
{
	// 조건 1: 속도가 가장 빠른 카드
	float MaxSpeed = 0.0f;
	for (TObjectPtr<ACharacterCard> Candidate : Candidates)
	{
		MaxSpeed = FMath::Max(MaxSpeed, Candidate->GetCharacterStat().CharSpeed);
	}
	
	TArray<ACharacterCard*> MaxSpeedCharacter;
	for (TObjectPtr<ACharacterCard> Candidate : Candidates)
	{
		if ((MaxSpeed - Candidate->GetCharacterStat().CharSpeed) < 0.0001f)
		{
			MaxSpeedCharacter.Add(Candidate);
		}
	}

	if (MaxSpeedCharacter.Num() == 1) return MaxSpeedCharacter[0];

	// 조건 2: 사람 카드 & 조건 3: 순번이 빠른 카드
	// Candidates가 항상 순번대로 들어오므로 가장 먼저 조건 2에 해당되는 카드를 반환
	for (TObjectPtr<ACharacterCard> Candidate : MaxSpeedCharacter)
	{
		if (Candidate->GetCardType() == CardType::person)
		{
			return Candidate;
		}
	}

	// 사람 카드가 없으면 속도가 빠른 카드 중 순번이 빠른 카드 반환
	return MaxSpeedCharacter[0];
}

ACharacterCard* ABattleManager::GetVictim(ACharacterCard* Attacker)
{
	int TeamIndex, CardIndex;
	if (FirstTeam.Find(Attacker) != INDEX_NONE)
	{
		TeamIndex = 1;
		CardIndex = FirstTeam.IndexOfByKey(Attacker);
	} else if (SecondTeam.Find(Attacker) != INDEX_NONE)
	{
		TeamIndex = 2;
		CardIndex = SecondTeam.IndexOfByKey(Attacker);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%d"), TeamIndex));
	TArray<int> OpponentWeight;
	int TotalWeight = 0;

	if (TeamIndex == 1)
	{
		float CardCenter = (FirstTeam.Num() - 1) / 2.0f;
		float OpponentCenter = (SecondTeam.Num() - 1) / 2.0f;
		for (int i = 0; i < SecondTeam.Num(); i++)
		{
			if (FMath::Abs((CardIndex - CardCenter) - (i - OpponentCenter)) < 0.6f)
			{
				OpponentWeight.Add(4);
				TotalWeight += 4;
			}
			else {
				OpponentWeight.Add(1);
				TotalWeight += 1;
			}
		}
	}
	else {
		float MyCenter = (SecondTeam.Num() - 1) / 2.0f;
		float OpponentCenter = (FirstTeam.Num() - 1) / 2.0f;
		for (int i = 0; i < FirstTeam.Num(); i++)
		{
			if (FMath::Abs((CardIndex - MyCenter) - (i - OpponentCenter)) < 0.6f)
			{
				OpponentWeight.Add(4);
				TotalWeight += 4;
			}
			else {
				OpponentWeight.Add(1);
				TotalWeight += 1;
			}
		}
	}

	int RandomNumber = FMath::RandRange(1, TotalWeight);
	int VictimIndex = -1;
	for (int i = 0; i < OpponentWeight.Num(); i++)
	{
		RandomNumber -= OpponentWeight[i];
		if (RandomNumber < 1)
		{
			VictimIndex = i;
			break;
		}
	}

	TObjectPtr<ACharacterCard> Victim;
	if (TeamIndex == 1)
	{
		Victim = SecondTeam[VictimIndex];
	}
	else {
		Victim = FirstTeam[VictimIndex];
	}

	return Victim;
}

void ABattleManager::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACharacterCard::StaticClass()))
	{
		TObjectPtr<ACharacterCard> OtherChar = Cast<ACharacterCard>(OtherActor);
		
		if (OtherChar->GetBattleState() != EBattleState::Idle) return;

		JoinBattle(OtherChar);
	}
}

// Called every frame
void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (SLGameMode->GetPlayState() != GamePlayState::PlayState) return;

	if (FirstTeam.Num() == 0 || SecondTeam.Num() == 0) EndBattle();

	TArray<ACharacterCard*> AttackerCandidate;
	if (CurrentAttacker == nullptr)
	{
		for (TObjectPtr<ACharacterCard> FirstChar : FirstTeam)
		{
			if (FirstChar->AddAttackGauge(DeltaTime))
			{
				AttackerCandidate.Add(FirstChar);
			}
		}
		for (TObjectPtr<ACharacterCard> SecondChar : SecondTeam)
		{
			if (SecondChar->AddAttackGauge(DeltaTime))
			{
				AttackerCandidate.Add(SecondChar);
			}
		}
	}
	if (AttackerCandidate.Num() > 0)
	{
		TObjectPtr<ACharacterCard> Attacker = GetAttacker(AttackerCandidate);
		Attack(Attacker, GetVictim(Attacker));
		Attacker->ResetAttackGauge();
	}
}

void ABattleManager::Attack(ACharacterCard* Attacker, ACharacterCard* Victim)
{
	CurrentAttacker = Attacker;
	CurrentVictim = Victim;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Attacker->GetName());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, Victim->GetName());

	FCardAnimationCallback Callback;
	Callback.BindUObject(this, &ABattleManager::DamageVictim);
	CurrentAttacker->MoveToAnother(CurrentVictim, Callback);
}

void ABattleManager::HandleDeath(ACharacterCard* DeadCard)
{
	LeaveBattle(DeadCard);
}

void ABattleManager::EndBattle()
{
	for (TObjectPtr<ACharacterCard> FirstChar : FirstTeam)
	{
		LeaveBattle(FirstChar);
	}

	for (TObjectPtr<ACharacterCard> SecondChar : SecondTeam)
	{
		LeaveBattle(SecondChar);
	}

	Destroy();
}

void ABattleManager::SetTeam(TArray<ACharacterCard*> Team1, TArray<ACharacterCard*> Team2)
{
	FirstTeam = Team1;
	SecondTeam = Team2;

	for (TObjectPtr<ACharacterCard> FirstChar : FirstTeam)
	{
		FirstChar->SetBattleState(EBattleState::Wait);
		FirstChar->ResetAttackGauge();
		FirstChar->OnDeath.AddDynamic(this, &ABattleManager::HandleDeath);
		FirstChar->OnLeaveBattle.AddDynamic(this, &ABattleManager::LeaveBattle);
	}

	for (TObjectPtr<ACharacterCard> SecondChar : SecondTeam)
	{
		SecondChar->SetBattleState(EBattleState::Wait);
		SecondChar->ResetAttackGauge();
		SecondChar->OnDeath.AddDynamic(this, &ABattleManager::HandleDeath);
		SecondChar->OnLeaveBattle.AddDynamic(this, &ABattleManager::LeaveBattle);
	}

	Relocate();
}

void ABattleManager::Relocate()
{
	float CardCenter = (FirstTeam.Num() - 1) / 2.0f;
	for (TObjectPtr<ACharacterCard> FirstChar : FirstTeam)
	{
		FVector Location = GetActorLocation();
		float IndexOffset = FirstTeam.Find(FirstChar) - CardCenter;
		Location.X += BattleCubeYOffset;
		Location.Y += BattleCubeWidthPerCard * IndexOffset;
		FirstChar->SetActorLocation(Location, false, nullptr, ETeleportType::ResetPhysics);
		FirstChar->GetVisualMesh()->SetSimulatePhysics(false);
	}

	CardCenter = (SecondTeam.Num() - 1) / 2.0f;
	for (TObjectPtr<ACharacterCard> SecondChar : SecondTeam)
	{
		FVector Location = GetActorLocation();
		float IndexOffset = SecondTeam.Find(SecondChar) - CardCenter;
		Location.X -= BattleCubeYOffset;
		Location.Y += BattleCubeWidthPerCard * IndexOffset;
		SecondChar->SetActorLocation(Location, false, nullptr, ETeleportType::ResetPhysics);
		SecondChar->GetVisualMesh()->SetSimulatePhysics(false);
	}

	float BattleCubeWidth = FMath::Max(FirstTeam.Num(), SecondTeam.Num()) * BattleCubeWidthPerCard + BattleCubeBaseWidth;

	BattleCube->SetRelativeScale3D(FVector(BattleCubeBaseHeight, BattleCubeWidth, 50.0f));
}

void ABattleManager::DamageVictim()
{
	FCharacterData AttackerStat = CurrentAttacker->GetCharacterStat();
	FCharacterData VictimStat = CurrentVictim->GetCharacterStat();

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
	CurrentVictim->CharacterDamage(FMath::CeilToInt(FinalDamage));

	FCardAnimationCallback Callback;
	Callback.BindUObject(this, &ABattleManager::MovebackCompleted);
	CurrentAttacker->MoveBack(Callback);
}

void ABattleManager::MovebackCompleted()
{
	CurrentAttacker->TargetCallback.Unbind();
	CurrentAttacker->ResetTargetLocation();
	CurrentAttacker = nullptr;
	CurrentVictim = nullptr;
}

void ABattleManager::JoinBattle(ACharacterCard* TargetCard)
{
	if (FirstTeam.Num() == 0) return;

	int TeamIndex = (TargetCard->GetCardType() == FirstTeam[0]->GetCardType()) ? 1 : 2;

	if (TeamIndex == 1)
	{
		FirstTeam.Add(TargetCard);
	}
	else {
		SecondTeam.Add(TargetCard);
	}

	TargetCard->SetBattleState(EBattleState::Wait);
	TargetCard->ResetAttackGauge();
	TargetCard->OnDeath.AddDynamic(this, &ABattleManager::HandleDeath);
	TargetCard->OnLeaveBattle.AddDynamic(this, &ABattleManager::LeaveBattle);

	Relocate();
}

void ABattleManager::LeaveBattle(ACharacterCard* TargetCard)
{
	FirstTeam.Remove(TargetCard);
	SecondTeam.Remove(TargetCard);

	if (UKismetSystemLibrary::IsValid(TargetCard))
	{
		TargetCard->SetBattleState(EBattleState::Idle);
		TargetCard->ResetAttackGauge();
		TargetCard->OnDeath.RemoveAll(this);
	}

	Relocate();
}