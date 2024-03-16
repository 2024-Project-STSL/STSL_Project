// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameModeBase.h"

void ASLGameModeBase::AddCardStack(ACardStack* CardStack)
{
	CardStacks.Add(CardStack);
}

void ASLGameModeBase::SetCardHighlight(bool CardHighlight, ACardStack* HoveringStack)
{
	IsCardHighlight = CardHighlight;
	if (IsCardHighlight)
	{
		if (HoveringStack == nullptr) return;
		for (ACardStack* CardStack : CardStacks)
		{
			if (CardStack != HoveringStack && ACardStack::IsCardStackable(HoveringStack, CardStack))
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
