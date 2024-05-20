// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuBase.h"

UMainMenuBase::UMainMenuBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UTexture2D>PlayIconOnRef(TEXT("/Script/Engine.Texture2D'/Game/UI/PlayIconOn.PlayIconOn'"));

    if (PlayIconOnRef.Succeeded())
    {
        PlayIconOn = PlayIconOnRef.Object;
        PlayBrushOn.SetResourceObject(PlayIconOn);
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D>PlayIconOffRef(TEXT("/Script/Engine.Texture2D'/Game/UI/PlayIconOff.PlayIconOff'"));

    if (PlayIconOffRef.Succeeded())
    {
        PlayIconOff = PlayIconOffRef.Object;
        PlayBrushOff.SetResourceObject(PlayIconOff);
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D>PauseIconOnRef(TEXT("/Script/Engine.Texture2D'/Game/UI/PauseIconOn.PauseIconOn'"));

    if (PauseIconOnRef.Succeeded())
    {
        PauseIconOn = PauseIconOnRef.Object;
        PauseBrushOn.SetResourceObject(PauseIconOn);
    }

    static ConstructorHelpers::FObjectFinder<UTexture2D>PauseIconOffRef(TEXT("/Script/Engine.Texture2D'/Game/UI/PauseIconOff.PauseIconOff'"));

    if (PauseIconOffRef.Succeeded())
    {
        PauseIconOff = PauseIconOffRef.Object;
        PauseBrushOff.SetResourceObject(PauseIconOff);
    }
}

void UMainMenuBase::UpdateIcon(GamePlayState GameState)
{
    PlayButton = Cast<UButton>(GetWidgetFromName(TEXT("PlayButton")));
    PauseButton = Cast<UButton>(GetWidgetFromName(TEXT("PauseButton")));
   
    FButtonStyle PlayButtonStyle = PlayButton->GetStyle();
    FButtonStyle PauseButtonStyle = PauseButton->GetStyle();

    switch (GameState) {
    case GamePlayState::PlayState:
        PlayButtonStyle.SetNormal(PlayBrushOn);
        PlayButtonStyle.SetHovered(PlayBrushOn);
        PlayButtonStyle.SetPressed(PlayBrushOn);
        PauseButtonStyle.SetNormal(PauseBrushOff);
        PauseButtonStyle.SetHovered(PauseBrushOff);
        PauseButtonStyle.SetPressed(PauseBrushOff);
        break;
    case GamePlayState::PauseState:
        PlayButtonStyle.SetNormal(PlayBrushOff);
        PlayButtonStyle.SetHovered(PlayBrushOff);
        PlayButtonStyle.SetPressed(PlayBrushOff);
        PauseButtonStyle.SetNormal(PauseBrushOn);
        PauseButtonStyle.SetHovered(PauseBrushOn);
        PauseButtonStyle.SetPressed(PauseBrushOn);
        break;
    default:
        break;
    }

    PlayButton->SetStyle(PlayButtonStyle);
    PauseButton->SetStyle(PauseButtonStyle);
}
