// Fill out your copyright notice in the Description page of Project Settings.

#include "SLGameModeBase.h"
#include <Kismet/GameplayStatics.h>
#include "MainMenuBase.h"

void ASLGameModeBase::Tick(float DeltaTime)
{
	if (CurrentPlayState == GamePlayState::PlayState)
	{
		Time += DeltaTime;
	}
	if (Time >= TimeForDay)
	{
		Time = 0;
		CurrentPlayState = GamePlayState::BreakState;
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void ASLGameModeBase::CreateMenu()
{
	MainMenu = CreateWidget(GetWorld(), LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/MainMenu.MainMenu_C'")));
	MainMenu->AddToViewport();
}

ASLGameModeBase::ASLGameModeBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WorldBorder.Add(TEXT("Left"), -3000.0f);
	WorldBorder.Add(TEXT("Right"), 3000.0f);
	WorldBorder.Add(TEXT("Down"), -2000.0f);
	WorldBorder.Add(TEXT("Up"), 2000.0f);

}

void ASLGameModeBase::PauseGame()
{
	if (CurrentPlayState == GamePlayState::PlayState)
	{
		CurrentPlayState = GamePlayState::PauseState;
	}
	Cast<UMainMenuBase>(MainMenu)->UpdateIcon(CurrentPlayState);
}

void ASLGameModeBase::ResumeGame()
{
	if (CurrentPlayState == GamePlayState::PauseState)
	{
		CurrentPlayState = GamePlayState::PlayState;
	}
	Cast<UMainMenuBase>(MainMenu)->UpdateIcon(CurrentPlayState);
}

float ASLGameModeBase::GetDayProgressPercent() const
{
	return FMath::Clamp(Time / TimeForDay, 0.0f, 1.0f);
}

void ASLGameModeBase::AddCardStack(ACardStack* CardStack)
{
	CardStacks.Add(CardStack);
}

void ASLGameModeBase::RemoveCardStack(ACardStack* CardStack)
{
	if (CardStack == DraggingStack)
	{
		DraggingStack = nullptr;
	}
	CardStacks.Remove(CardStack);
}


TMap<FString, float> ASLGameModeBase::GetWorldBorder(bool bExcludeBuyArea) const
{
	if (bExcludeBuyArea)
	{
		TMap<FString, float> Border = WorldBorder;
		Border["Up"] -= BuyAreaHeight;
		return Border;
	}
	else {
		return WorldBorder;
	}
}

void ASLGameModeBase::SetCardHighlight(bool bCardHighlight, ACardStack* NewDraggingStack)
{
	bIsCardHighlight = bCardHighlight;
	if (bIsCardHighlight)
	{
		DraggingStack = NewDraggingStack;
		if (DraggingStack == nullptr) return;
		for (ACardStack* CardStack : CardStacks)
		{
			if (CardStack != DraggingStack && ACardStack::GetCardStackable(DraggingStack, CardStack))
			{
				CardStack->GetLastCard()->GetVisualMesh()->SetRenderCustomDepth(true);
			}
		}
	} else {
		for (ACardStack* CardStack : CardStacks)
		{
			CardStack->GetLastCard()->GetVisualMesh()->SetRenderCustomDepth(false);
		}
	}
}

ACardStack* ASLGameModeBase::SpawnCard(FVector Location, int CardID)
{
	DraggingStack = nullptr;

	AActor* NewCardStackActor = GetWorld()->SpawnActor
	(
		ACardStack::StaticClass(),
		&Location
	);

	if (NewCardStackActor == nullptr) return nullptr;
	ACardStack* NewCardStack = Cast<ACardStack>(NewCardStackActor);

	AActor* NewCardActor = GetWorld()->SpawnActor
	(
		ACard::StaticClass(),
		&Location
	);

	if (NewCardActor == nullptr) return nullptr;
	ACard* NewCard = Cast<ACard>(NewCardActor);

	NewCard->SetCardID(CardID);
	NewCardStack->AddCard(NewCardActor);

	return NewCardStack;
}

ACardStack* ASLGameModeBase::SpawnCardPack(FVector Location, int PackID)
{
	DraggingStack = nullptr;

	AActor* NewCardStackActor = GetWorld()->SpawnActor
	(
		ACardStack::StaticClass(),
		&Location
	);

	if (NewCardStackActor == nullptr) return nullptr;
	ACardStack* NewCardStack = Cast<ACardStack>(NewCardStackActor);

	AActor* NewCardActor = GetWorld()->SpawnActor
	(
		ACardPack::StaticClass(),
		&Location
	);

	if (NewCardActor == nullptr) return nullptr;
	ACardPack* NewCard = Cast<ACardPack>(NewCardActor);

	NewCard->SetCardID(PackID);
	NewCardStack->AddCard(NewCardActor);

	return NewCardStack;
}
