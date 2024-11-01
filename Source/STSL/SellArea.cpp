// Fill out your copyright notice in the Description page of Project Settings.


#include "SellArea.h"
#include "SLGameModeBase.h"
#include "CardStack.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ASellArea::ASellArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UFont> HanbitFont(TEXT("/Script/Engine.Font'/Game/Fonts/Hanbit_Offline.Hanbit_Offline'"));

    if (HanbitFont.Succeeded())
    {
        CardFont = HanbitFont.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterial> HanbitFontMat(TEXT("/Script/Engine.Material'/Game/Fonts/Hanbit_Offline_Material.Hanbit_Offline_Material'"));

    if (HanbitFontMat.Succeeded())
    {
        CardFontMat = HanbitFontMat.Object;
    }

    SellText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TitleText"));
    SellText->SetHorizontalAlignment(EHTA_Center);
    SellText->SetVerticalAlignment(EVRTA_TextCenter);
    SellText->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 6.0f), FRotator(90.0f, 0.0f, 180.0f));
    SellText->SetTextRenderColor(FColor::White);
    SellText->SetWorldSize(FontSize);
    SellText->SetMaterial(0, CardFontMat);
    SellText->SetFont(CardFont);
    FText SellAreaText = FText::FromStringTable("/Game/DataTable/MainStringTable.MainStringTable", "SellArea_Text");
    SellText->SetText(SellAreaText);
    SellText->SetupAttachment(VisualMesh);
}

void ASellArea::SellCard(ACardStack* CardStack)
{
    if (CardStack->GetIsSellable())
    {
        int PriceSum = CardStack->GetPriceSum();
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Sell card at price: %d"), PriceSum));
        CardStack->RemoveAllCards(true);
        SpawnCoinStack(GetActorLocation(), PriceSum);
        OnSellCard.Broadcast();
    }
    else {
        CardStack->GetFirstCard()->UpdateGroundPosition();
    }
}

