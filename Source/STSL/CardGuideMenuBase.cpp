// Fill out your copyright notice in the Description page of Project Settings.


#include "CardGuideMenuBase.h"
#include <Data/RecipeData.h>

void UCardGuideMenuBase::NativeConstruct()
{
    for (const auto& Row : CardDataTable->GetRowMap())
    {
        int RecipeID = FCString::Atoi(*Row.Key.ToString());

        const uint8* RowData = Row.Value;

        FCardData* CardData = reinterpret_cast<FCardData*>(const_cast<uint8*>(RowData));

        if (CardData->CardCode == 0) continue;

        UCardEntryData* EntryData = ConvertToEntryData(CardData);

        CardListView->AddItem(EntryData);
    }

    CardListView->OnItemClicked().AddUObject(this, &UCardGuideMenuBase::OnCardClicked);

    ClearCardDetail();
}

void UCardGuideMenuBase::OnCardClicked(UObject* CardObject)
{
    ShowCardDetail(Cast<UCardEntryData>(CardObject)->CardCode);
}

UCardGuideMenuBase::UCardGuideMenuBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/DataTable/CardDB.CardDB'"));
    if (DataTableRef.Succeeded())
    {
        CardDataTable = DataTableRef.Object;
    }

    static ConstructorHelpers::FObjectFinder<UDataTable> CraftDataTableRef(TEXT("/Script/Engine.DataTable'/Game/DataTable/RecipeDB.RecipeDB'"));
    if (CraftDataTableRef.Succeeded())
    {
        CraftingRecipeTable = CraftDataTableRef.Object;
    }
}

UCardEntryData* UCardGuideMenuBase::ConvertToEntryData(FCardData* CardData)
{
    UCardEntryData* NewEntryData = NewObject<UCardEntryData>();

    NewEntryData->CardName = CardData->CardName;
    NewEntryData->CardCode = CardData->CardCode;

    return NewEntryData;
}

void UCardGuideMenuBase::ClearCardDetail()
{
    CardName->SetText(FText::GetEmpty());
    CardDetail->SetText(FText::GetEmpty());
    CardIngrediant1->SetText(FText::GetEmpty());
    CardIngrediant2->SetText(FText::GetEmpty());
    CardIngrediant3->SetText(FText::GetEmpty());
}

void UCardGuideMenuBase::ShowCardDetail(int CardCode)
{
    ClearCardDetail();

    FCardData CardData;

    if (CardDataTable != nullptr)
    {
        FName RowName = FName(*FString::FromInt(CardCode));
        FCardData* RowData = CardDataTable->FindRow<FCardData>(RowName, TEXT(""));
        if (RowData != nullptr) CardData = *RowData;
    }

    CardName->SetText(FText::FromString(CardData.CardName));
    CardDetail->SetText(FText::FromString(CardData.CardScript));

    TArray<FRecipeData*> RecipeRows;

    TArray<FString> ReqCardName;
    TArray<int> ReqCardAmount;

    for (const auto& Row : CraftingRecipeTable->GetRowMap())
    {
        int RecipeID = FCString::Atoi(*Row.Key.ToString());

        const uint8* RecipeRowData = Row.Value;

        FRecipeData* RecipeData = reinterpret_cast<FRecipeData*>(const_cast<uint8*>(RecipeRowData));
        
        if (RecipeData->CardCode == CardCode)
        {
            for (int ReqCode : RecipeData->ReqCardCode)
            {
                FName RowName = FName(*FString::FromInt(ReqCode));
                FCardData* RowData = CardDataTable->FindRow<FCardData>(RowName, TEXT(""));
                if (RowData != nullptr) ReqCardName.Add(RowData->CardName);
            }

            ReqCardAmount = RecipeData->ReqValue;

            break;
        }
    }

    FString IngrediantString;
 
    switch (ReqCardName.Num())
    {
    case 3:
        IngrediantString = ReqCardName[2].Append(FString::Printf(TEXT(" X %d"), ReqCardAmount[2]));
        CardIngrediant3->SetText(FText::FromString(IngrediantString));
    case 2:
        IngrediantString = ReqCardName[1].Append(FString::Printf(TEXT(" X %d"), ReqCardAmount[1]));
        CardIngrediant2->SetText(FText::FromString(IngrediantString));
    case 1:
        IngrediantString = ReqCardName[0].Append(FString::Printf(TEXT(" X %d"), ReqCardAmount[0]));
        CardIngrediant1->SetText(FText::FromString(IngrediantString));
    default:
        break;
    }

    if (ReqCardName.Num() == 0)
    {
        FText UncraftableText = FText::FromStringTable("/Game/DataTable/MainStringTable.MainStringTable", "UI_CardGuide_Uncraftable");
        CardIngrediant1->SetText(UncraftableText);
    }
}
