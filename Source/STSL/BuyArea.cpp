// Fill out your copyright notice in the Description page of Project Settings.


#include "BuyArea.h"
#include "CardStack.h"
#include "SLGameModeBase.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ABuyArea::ABuyArea()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/CardPackDB.CardPackDB'"));
    if (DataTable.Succeeded())
    {
        CardPackDataTable = DataTable.Object;
    }

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

    PackNameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameText"));
    PackNameText->SetHorizontalAlignment(EHTA_Center);
    PackNameText->SetVerticalAlignment(EVRTA_TextCenter);
    PackNameText->SetRelativeLocationAndRotation(FVector(100.0f, 0.0f, 6.0f), FRotator(90.0f, 0.0f, 180.0f));
    PackNameText->SetTextRenderColor(FColor::White);
    PackNameText->SetWorldSize(NameFontSize);
    PackNameText->SetMaterial(0, CardFontMat);
    PackNameText->SetFont(CardFont);
    PackNameText->SetText(FText::FromString(CardPackData.PackName));
    PackNameText->SetupAttachment(VisualMesh);

    PackPriceText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PriceText"));
    PackPriceText->SetHorizontalAlignment(EHTA_Center);
    PackPriceText->SetVerticalAlignment(EVRTA_TextCenter);
    PackPriceText->SetRelativeLocationAndRotation(FVector(-150.0f, 0.0f, 6.0f), FRotator(90.0f, 0.0f, 180.0f));
    PackPriceText->SetTextRenderColor(FColor::White);
    PackPriceText->SetWorldSize(PriceFontSize);
    PackPriceText->SetMaterial(0, CardFontMat);
    PackPriceText->SetFont(CardFont);
    PackPriceText->SetText(FText::AsNumber(CardPackData.PackPrice));
    PackPriceText->SetupAttachment(VisualMesh);

    CurrentCardPrice = CardPackData.PackPrice;
    LoadArea();
}

void ABuyArea::BeginPlay()
{
    LoadArea();
}

void ABuyArea::LoadArea()
{
    if (CardPackDataTable != nullptr)
    {
        FName RowName = FName(*FString::FromInt(CardPackData.PackCode));
        FCardPackData* RowData = CardPackDataTable->FindRow<FCardPackData>(RowName, TEXT(""));
        if (RowData != nullptr) CardPackData = *RowData;
    }

    PackNameText->SetText(FText::FromString(CardPackData.PackName));
    PackNameText->SetWorldSize(FMath::Clamp(NameFontSize * 4.0f / CardPackData.PackName.Len(), NameFontSize / 8.0f, NameFontSize));
    CurrentCardPrice = CardPackData.PackPrice;
    UpdatePriceText();
}

void ABuyArea::UpdatePriceText()
{
    if (CurrentCardPrice >= 0)
    {
        PackPriceText->SetText(FText::AsNumber(CurrentCardPrice));
    }
    else {
        PackPriceText->SetText(FText::FromString(""));
    }
}

void ABuyArea::BuyCard(ACardStack* CardStack)
{
    if (CardStack->GetIsCoinStack())
    {
        // 코인 가격이 1이므로 코인 가격 = 개수
        CurrentCardPrice -= CardStack->GetPriceSum();
        CardStack->RemoveAllCards(true);

        ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
        int Charge;
        FVector Location = GetActorLocation();

        if (CurrentCardPrice <= 0)
        {
            Charge = CurrentCardPrice * -1;
            SpawnCoinStack(Location, Charge);
            CurrentCardPrice = CardPackData.PackValue;
            Location.X -= OutputOffset * 1.2f;
            SLGameMode->SpawnCardPack(Location, CardPackData.PackCode);
        }

        UpdatePriceText();
    }
    else {
        CardStack->GetFirstCard()->UpdateGroundPosition();
    }
}