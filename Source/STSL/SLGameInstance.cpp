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