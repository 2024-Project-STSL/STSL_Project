// Fill out your copyright notice in the Description page of Project Settings.

#include "SLGameModeBase.h"

ASLGameModeBase::ASLGameModeBase()
{
	WorldBorder.Add(TEXT("Left"), -3000.0f);
	WorldBorder.Add(TEXT("Right"), 3000.0f);
	WorldBorder.Add(TEXT("Down"), -2000.0f);
	WorldBorder.Add(TEXT("Up"), 2000.0f);
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
