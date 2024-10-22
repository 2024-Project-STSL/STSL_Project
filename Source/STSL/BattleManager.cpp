// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include <Kismet/GameplayStatics.h>
#include "SLGameModeBase.h"
#include "DamageIndicatorBase.h"

// Sets default values
ABattleManager::ABattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BattleCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//RootComponent = BattleCube;

	HighlightCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HighlightMesh"));
	HighlightCube->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BattleCubeMesh(TEXT("/Script/Engine.StaticMesh'/Game/Mesh/BattleCube.BattleCube'"));

	if (BattleCubeMesh.Succeeded())
	{
		BattleCube->SetStaticMesh(BattleCubeMesh.Object);
		BattleCube->SetSimulatePhysics(false);
		BattleCube->SetCastShadow(false);
		BattleCube->SetCollisionProfileName("OverlapAll");

		HighlightCube->SetStaticMesh(BattleCubeMesh.Object);
		HighlightCube->SetSimulatePhysics(false);
		HighlightCube->SetCollisionProfileName("OverlapAll");
		HighlightCube->SetRenderInMainPass(false);
		HighlightCube->SetRenderInDepthPass(false);
		HighlightCube->SetRenderCustomDepth(true);
		HighlightCube->SetCastShadow(false);

		static ConstructorHelpers::FObjectFinder<UMaterial> BattleCubeMaterial(TEXT("/Script/Engine.Material'/Game/Material/BattleCubeMaterial.BattleCubeMaterial'"));

		if (BattleCubeMaterial.Succeeded())
		{
			BattleCube->SetMaterial(0, BattleCubeMaterial.Object);
		}
	}
	
	AttackArrow = CreateDefaultSubobject<UWidgetComponent>(TEXT("AttackArrowWidget"));
	AttackArrow->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UUserWidget> AttackArrowRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/AttackArrow.AttackArrow_C'"));
	if (AttackArrowRef.Succeeded())
	{
		AttackArrow->SetWidgetClass(AttackArrowRef.Class);
		AttackArrow->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
		AttackArrow->SetDrawSize(FVector2D(200.0f, 200.0f));
		AttackArrow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttackArrow->SetCastShadow(false);
	}

	DamageIndicator = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageIndicatorWidget"));
	DamageIndicator->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UUserWidget> DamageIndicatorRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/DamageIndicator.DamageIndicator_C'"));
	if (DamageIndicatorRef.Succeeded())
	{
		DamageIndicator->SetWidgetClass(DamageIndicatorRef.Class);
		DamageIndicator->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.6f), FRotator(90.0f, 0.0f, 180.0f));
		DamageIndicator->SetDrawSize(FVector2D(200.0f, 200.0f));
		DamageIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DamageIndicator->SetCastShadow(false);
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
	HighlightCube->SetCustomDepthStencilValue(2);
	AttackArrow->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	DamageIndicator->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
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
	int TeamIndex = INDEX_NONE, CardIndex = INDEX_NONE;
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
	if (IsPendingKillPending()) return;
	if (OtherActor->IsA(ACharacterCard::StaticClass()))
	{
		TObjectPtr<ACharacterCard> OtherChar = Cast<ACharacterCard>(OtherActor);

		if (OtherChar->GetBattleState() != EBattleState::Idle) return;

		JoinBattle(OtherChar);
	}
	else if (OtherActor->IsA(ABattleManager::StaticClass()))
	{
		MergeBattle(this, Cast<ABattleManager>(OtherActor));
	}
}

void ABattleManager::MergeBattle(ABattleManager* Battle1, ABattleManager* Battle2)
{
	if (Battle1 == Battle2) return;
	for (TObjectPtr<ACharacterCard> Battle2Team1 : Battle2->FirstTeam)
	{
		Battle2->LeaveBattle(Battle2Team1);
		Battle1->JoinBattle(Battle2Team1);
	}
	for (TObjectPtr<ACharacterCard> Battle2Team2 : Battle2->SecondTeam)
	{
		Battle2->LeaveBattle(Battle2Team2);
		Battle1->JoinBattle(Battle2Team2);
	}
	Battle2->Destroy();
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
			if (FirstChar->FindEffect(EffectCode::Stun) != INDEX_NONE) continue;

			if (FirstChar->AddAttackGauge(DeltaTime))
			{
				AttackerCandidate.Add(FirstChar);
			}
		}
		for (TObjectPtr<ACharacterCard> SecondChar : SecondTeam)
		{
			if (SecondChar->FindEffect(EffectCode::Stun) != INDEX_NONE) continue;

			if (SecondChar->AddAttackGauge(DeltaTime))
			{
				AttackerCandidate.Add(SecondChar);
			}
		}
	}
	for (ACharacterCard* AttackCandidate : AttackerCandidate)
	{
		if (AttackCandidate->GetBattleState() == EBattleState::Idle)
		{
			AttackCandidate->SetBattleState(EBattleState::Ready);
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

	CurrentAttacker->SetBattleState(EBattleState::Attack);

	FCardAnimationCallback Callback;
	Callback.BindUObject(this, &ABattleManager::DamageVictim);
	CurrentAttacker->MoveToAnother(CurrentVictim, Callback, AttackMoveDistance);

	FVector AttackerLocation = CurrentAttacker->GetActorLocation();
	FVector VictimLocation = CurrentVictim->GetActorLocation();

	AttackerLocation.Z = VictimLocation.Z;

	FRotator LookAtRotation = (VictimLocation - AttackerLocation).Rotation();
	LookAtRotation.Add(90.0f, 0.0f, 180.0f);

	FVector ArrowLocation = (AttackerLocation + VictimLocation) / 2.0f;
	ArrowLocation.Z = 10.0f;

	AttackArrow->SetWorldLocationAndRotation(ArrowLocation, LookAtRotation);
	AttackArrow->GetWidget()->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void ABattleManager::HandleDeath(ACharacterCard* DeadCard)
{
	LeaveBattle(DeadCard);
}

void ABattleManager::EndBattle()
{
	for (int i = FirstTeam.Num() - 1; i >= 0; i--)
	{
		auto FirstChar = FirstTeam[i];
		LeaveBattle(FirstChar);
	}

	for (int i = SecondTeam.Num() - 1; i >= 0; i--)
	{
		auto SecondChar = SecondTeam[i];
		LeaveBattle(SecondChar);
	}

	Destroy();
}

void ABattleManager::SetBattle(TArray<ACharacterCard*>& Team1, TArray<ACharacterCard*>& Team2, int32 NewBattleID, bool bResetChar)
{
	FirstTeam = Team1;
	SecondTeam = Team2;
	BattleID = NewBattleID;

	for (auto FirstChar : FirstTeam)
	{
		if (bResetChar)
		{
			FirstChar->SetBattleState(EBattleState::Wait);
			FirstChar->ResetAttackGauge();
		}
		if (NewBattleID != -1) FirstChar->SetBattleID(NewBattleID);
		FirstChar->OnDeath.AddUniqueDynamic(this, &ABattleManager::HandleDeath);
		FirstChar->OnLeaveBattle.AddUniqueDynamic(this, &ABattleManager::LeaveBattle);
	}

	for (auto SecondChar : SecondTeam)
	{
		if (bResetChar)
		{
			SecondChar->SetBattleState(EBattleState::Wait);
			SecondChar->ResetAttackGauge();
		}
		if (NewBattleID != -1) SecondChar->SetBattleID(NewBattleID);
		SecondChar->OnDeath.AddUniqueDynamic(this, &ABattleManager::HandleDeath);
		SecondChar->OnLeaveBattle.AddUniqueDynamic(this, &ABattleManager::LeaveBattle);
	}

	Relocate();
}

void ABattleManager::SetBattleID(int32 NewBattleID)
{
	BattleID = NewBattleID;
	for (auto FirstChar : FirstTeam) FirstChar->SetBattleID(NewBattleID);
	for (auto SecondChar : SecondTeam) SecondChar->SetBattleID(NewBattleID);
}

FVector ABattleManager::GetCardPosition(int TeamIndex, int CardIndex) const
{
	FVector Position = GetActorLocation();

	float CardCenter;

	if (TeamIndex == 1)
	{
		CardCenter = (FirstTeam.Num() - 1) / 2.0f;
	}
	else {
		CardCenter = (SecondTeam.Num() - 1) / 2.0f;
	}
	
	float IndexOffset = CardIndex - CardCenter;

	Position.X += BattleCubeXOffset * (TeamIndex * 2 - 3);
	Position.Y += BattleCubeWidthPerCard * IndexOffset;

	return Position;
}

FVector ABattleManager::GetCardPosition(ACharacterCard* TargetCard) const
{
	int CardIndex, TeamIndex;
	CardIndex = FirstTeam.Find(TargetCard);
	if (CardIndex != -1)
	{
		TeamIndex = 1;
	}
	else {
		CardIndex = SecondTeam.Find(TargetCard);
		TeamIndex = 2;
	}
	return GetCardPosition(TeamIndex, CardIndex);
}

void ABattleManager::Relocate()
{
	for (TObjectPtr<ACharacterCard> FirstChar : FirstTeam)
	{
		FVector Location = GetCardPosition(1, FirstTeam.Find(FirstChar));
		FirstChar->SetActorLocation(Location, false, nullptr, ETeleportType::ResetPhysics);
		FirstChar->GetVisualMesh()->SetSimulatePhysics(false);
	}
	for (TObjectPtr<ACharacterCard> SecondChar : SecondTeam)
	{
		FVector Location = GetCardPosition(2, SecondTeam.Find(SecondChar));
		SecondChar->SetActorLocation(Location, false, nullptr, ETeleportType::ResetPhysics);
		SecondChar->GetVisualMesh()->SetSimulatePhysics(false);
	}

	float BattleCubeWidth = FMath::Max(FirstTeam.Num(), SecondTeam.Num()) * BattleCubeWidthPerCard + BattleCubeBaseWidth;

	BattleCube->SetRelativeScale3D(FVector(BattleCubeBaseHeight, BattleCubeWidth, 50.0f));
	HighlightCube->SetRelativeScale3D(BattleCube->GetRelativeScale3D());
	HighlightCube->SetWorldLocation(BattleCube->GetComponentLocation());
}

void ABattleManager::DamageVictim()
{
	FCharacterData AttackerStat = CurrentAttacker->GetCharacterStat();
	FCharacterData VictimStat = CurrentVictim->GetCharacterStat();

	CurrentAttacker->SetBattleState(EBattleState::MoveBack);
	CurrentVictim->SetBattleState(EBattleState::Hit);

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
	bool bDead = CurrentVictim->CharacterDamage(FMath::RoundToInt(FinalDamage));

	if (!bDead)
	{
		FVector IndicatorLocation = GetCardPosition(CurrentVictim);
		DamageIndicator->SetWorldLocation(IndicatorLocation + DamageIndicatorOffset);
		Cast<UDamageIndicatorBase>(DamageIndicator->GetWidget())->SetDamageText(FinalDamage);
		DamageIndicator->GetWidget()->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	FCardAnimationCallback Callback;
	Callback.BindUObject(this, &ABattleManager::MovebackCompleted);
	CurrentAttacker->MoveBack(Callback);
}

void ABattleManager::MovebackCompleted()
{
	DamageIndicator->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	AttackArrow->GetWidget()->SetVisibility(ESlateVisibility::Hidden);

	CurrentAttacker->SetBattleState(EBattleState::Wait);
	CurrentVictim->SetBattleState(EBattleState::Wait);

	CurrentAttacker->TargetCallback.Unbind();
	CurrentAttacker->ResetTargetLocation();
	CurrentAttacker = nullptr;
	CurrentVictim = nullptr;

	Relocate();
}

void ABattleManager::JoinBattle(ACharacterCard* TargetCard, bool bResetChar)
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

	if (bResetChar) TargetCard->SetBattleState(EBattleState::Wait);
	if (bResetChar) TargetCard->ResetAttackGauge();
	TargetCard->SetBattleID(BattleID);
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
		TargetCard->ResetTargetLocation();
		TargetCard->OnDeath.Remove(this, TEXT("HandleDeath"));
		TargetCard->OnLeaveBattle.Remove(this, TEXT("LeaveBattle"));
	}

	Relocate();
}

TArray<ACharacterCard*> ABattleManager::GetAllCharacters() const
{
	TArray<ACharacterCard*> AllCharacters;
	AllCharacters.Append(FirstTeam);
	AllCharacters.Append(SecondTeam);

	return AllCharacters;
}
