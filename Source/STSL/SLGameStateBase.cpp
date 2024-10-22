// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameStateBase.h"
#include "SLSaveGame.h"
#include <SLGameInstance.h>

USLSaveGame* ASLGameStateBase::GetSaveGame() const
{
	USLSaveGame* SaveGame = NewObject<USLSaveGame>();

	USLGameInstance* GameInstance = Cast<USLGameInstance>(GetGameInstance());

	SaveGame->MasterVolume = GameInstance->MasterVolume;
	SaveGame->BackgroundVolume = GameInstance->BackgroundVolume;
	SaveGame->EffectVolume = GameInstance->EffectVolume;

	SaveGame->Day = Day;
	SaveGame->Time = Time;
	SaveGame->CardLimit = CardLimit;
	SaveGame->CurrentPlayState = CurrentPlayState;
	SaveGame->MaxBattleID = MaxBattleID;

	for (auto CardStack : CardStacks)
	{
		FCardStackSaveData CardStackData;
		
		CardStackData.CardLocation = CardStack->GetFirstCard()->GetActorLocation();
		CardStackData.CraftingProgress = CardStack->GetCraftingProgress();

		for (auto Card : CardStack->GetAllCards())
		{
			CardStackData.AddCard(Card);
		}

		SaveGame->AllCardStacks.Add(CardStackData);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%d Stacks saved."), SaveGame->AllCardStacks.Num()));

	return SaveGame;
}
