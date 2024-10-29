// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameInstance.h"
#include <Kismet/GameplayStatics.h>

USLGameInstance::USLGameInstance()
{
	USLSaveGame* SaveGame = Cast<USLSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("STSLSave"), 0));

	if (SaveGame != nullptr)
	{
		MasterVolume = SaveGame->MasterVolume;
		BackgroundVolume = SaveGame->BackgroundVolume;
		EffectVolume = SaveGame->EffectVolume;
	}
}

void USLGameInstance::SaveSettings()
{
	if (IsSaving())
	{
		bPendingSaveSetting = true;
		return;
	}
	USLSaveGame* SaveGame = Cast<USLSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("STSLSave"), 0));

	if (SaveGame == nullptr)
	{
		SaveGame = NewObject<USLSaveGame>();
	}

	SaveGame->MasterVolume = MasterVolume;
	SaveGame->BackgroundVolume = BackgroundVolume;
	SaveGame->EffectVolume = EffectVolume;

	UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("STSLSave"), 0);
}

void USLGameInstance::SaveGame(USLSaveGame* SaveGame)
{

	FAsyncSaveGameToSlotDelegate SaveCompleteDelegate;
	SaveCompleteDelegate.BindUFunction(this, TEXT("HandleSaveComplete"));

	UGameplayStatics::AsyncSaveGameToSlot(SaveGame, TEXT("STSLSave"), 0, SaveCompleteDelegate);
}

void USLGameInstance::HandleSaveComplete(FString& SlotName, int32 UserIndex, bool bSuccess)
{
	if (!bSuccess)
	{
		UE_LOG(LogClass, Warning, TEXT("Error occurred at game saving"));
	}
	bIsSaving = false;

	if (bPendingSaveSetting)
	{
		bPendingSaveSetting = false;
		SaveSettings();
	}

	OnAsyncSavingCompleted.Broadcast();
}
