// Fill out your copyright notice in the Description page of Project Settings.


#include "CardPack.h"
#include "CardStack.h"
#include "Card.h"
#include "SLGameModeBase.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>

void ACardPack::LoadCard()
{
    if (CardPackDataTable != nullptr)
    {
        FName RowName = FName(*FString::FromInt(CardPackData.PackCode));
        FCardPackData* RowData = CardPackDataTable->FindRow<FCardPackData>(RowName, TEXT(""));
        if (RowData != nullptr) CardPackData = *RowData;
    }

    // 모든 카드팩은 판매 불가
    CardData.CardPrice = -1;
    CardData.CardType = CardType::pack;
    CardData.AddType = AddType::nope;

    TitleText->SetText(FText::FromString(CardPackData.PackName));
    TitleText->SetTextRenderColor(FColor::White);
    TitleText->SetWorldSize(FMath::Clamp(FontSize * 6.0f / CardData.CardName.Len(), FontSize / 8.0f, FontSize));
   
    SellPriceText->SetText(FText::FromString(""));
    AddTypeText->SetText(FText::FromString(""));

    PriceIcon->SetText(FText::FromString(""));

    // 동적으로 카드 이미지 로드
    if (CardData.CardCode)
    {
        FString MaterialPath = "/Script/Engine.Material'/Game/CardImages/Pack_";
        MaterialPath += FString::Printf(TEXT("%d_Mat.%d_Mat'"), CardData.CardCode, CardData.CardCode);
        UMaterial* CardMaterial = LoadObject<UMaterial>(nullptr, *MaterialPath);
        if (CardMaterial)
        {
            CardImageWidget->SetMaterial(0, CardMaterial);
            CardImageWidget->RequestRedraw();
        }
    }
}

ACardPack::ACardPack()
{
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> PackMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Material/CardPackMaterial.CardPackMaterial'"));

    if (PackMat.Succeeded())
    {
        CardPackMat = PackMat.Object;
    }

    VisualMesh->SetMaterial(0, CardPackMat);

    static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/CardPackDB.CardPackDB'"));
    if (DataTable.Succeeded())
    {
        CardPackDataTable = DataTable.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> CardPackOpenSound(TEXT("/Script/Engine.SoundCue'/Game/Sounds/SE_OpenCardpack_Cue.SE_OpenCardpack_Cue'"));
    if (CardPackOpenSound.Succeeded())
    {
        SoundOpen = CardPackOpenSound.Object;
    }

    PushVector.Z = -1000.0;
    LoadCard();
}

ACardPack::ACardPack(int32 PackID)
{
    CardData.CardCode = PackID + 10000;
    CardPackData.PackCode = PackID;
    ACardPack();

}

void ACardPack::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (FloatingTime > 0)
    {
        FloatingTime -= DeltaTime;
        if (FloatingTime <= 0)
        {
            EndHover();
        }
    }
}

void ACardPack::SetCardID(int32 NewCardID)
{
    CardData.CardCode = NewCardID + 10000;
    CardPackData.PackCode = NewCardID;
    LoadCard(); 
}

void ACardPack::StartCardDrag()
{
    Super::StartCardDrag();
    StartDragPos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
}

void ACardPack::EndCardDrag()
{
    Super::EndCardDrag();
    if ((StartDragPos - UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld())).Length() < OpenThreshold)
    {
        VisualMesh->SetPhysicsLinearVelocity(FVector::Zero());
        SpawnCard();
        StartHover(10.0f);
    }
}

void ACardPack::SpawnCard()
{
    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

    ACardStack* NewCardStack = nullptr;
    
    FVector Location = GetActorLocation();
    Location.Z = 11.0f;
    SetActorLocation(Location);
    Location.Z -= 5.0f;

    int TotalWeight = 0;
    for (int CardWeight : CardPackData.CardWeight)
    {
        TotalWeight += CardWeight;
    }

    int RandomNumber = FMath::RandRange(1, TotalWeight);
    for (int i = 0; i < CardPackData.CardList.Num(); i++)
    {
        RandomNumber -= CardPackData.CardWeight[i];
        if (RandomNumber < 1)
        {
            NewCardStack = SLGameMode->SpawnCard(Location, CardPackData.CardList[i]);
            break;
        }
    }

    NewCardStack->GetFirstCard()->Push(FVector(8000.0f, 8000.0f, 0.0f));
    UGameplayStatics::PlaySoundAtLocation(this, SoundOpen, GetActorLocation());


    CardPackData.PackValue--;
    if (CardPackData.PackValue == 0)
    {
        Cast<ACardStack>(CardStack)->RemoveCard(this, true);
    }
}