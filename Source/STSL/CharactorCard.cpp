// Fill out your copyright notice in the Description page of Project Settings.


#include "CharactorCard.h"
#include "SLGameModeBase.h"
#include <Kismet/GameplayStatics.h>

ACharactorCard::ACharactorCard()
{
    CurrentMoveCooldown = MoveCooldown;

    HealthIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthIcon"));
    HealthIcon->SetupAttachment(VisualMesh);
    HealthIcon->SetRelativeLocationAndRotation(FVector(-240.0f, 140.0f, 0.55f), FRotator(90.0f, 0.0f, 180.0f));
    // �����Ϸ� ������ ī�� ��ü�� ������ ��ȭ�� ����
    HealthIcon->SetDrawSize(FVector2D(90.0f / GetActorScale().X, 90.0f / GetActorScale().X));

    static ConstructorHelpers::FObjectFinder<UDataTable> CharDataTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/CharactorDB.CharactorDB'"));
    if (CharDataTable.Succeeded())
    {
        CharactorDataTable = CharDataTable.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInstance> NeturalMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Material/NeturalCardMaterial.NeturalCardMaterial'"));
    if (NeturalMat.Succeeded())
    {
        NeturalCardMaterial = NeturalMat.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInstance> EnemyMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Material/EnemyCardMaterial.EnemyCardMaterial'"));
    if (EnemyMat.Succeeded())
    {
        EnemyCardMaterial = EnemyMat.Object;
    }
}

ACharactorCard::ACharactorCard(int32 CardID)
{
	ACharactorCard();
	this->CardData.CardCode = CardID;
}

void ACharactorCard::LoadCard()
{
    Super::LoadCard();

    FName RowName = FName(*FString::FromInt(CardData.CardCode));
    FCharactorData* CharRowData = CharactorDataTable->FindRow<FCharactorData>(RowName, TEXT(""));
    Health = CharRowData->CharHealth;

    if (CardData.CardType == CardType::netural)
    {
        VisualMesh->SetMaterial(0, NeturalCardMaterial);
    }

    if (CardData.CardType == CardType::enemy)
    {
        VisualMesh->SetMaterial(0, EnemyCardMaterial);
    }

    AddTypeText->SetText(FText::AsNumber(Health));
    // TODO : ��Ʈ ���������� ����
    FString MaterialPath = "/Script/Engine.Material'/Game/CardImages/14_Mat.14_Mat'";
    // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, MaterialPath);
    UMaterial* HealthIconMaterial = LoadObject<UMaterial>(nullptr, *MaterialPath);
    if (HealthIconMaterial)
    {
        HealthIcon->SetMaterial(0, HealthIconMaterial);
        HealthIcon->RequestRedraw();
    }
}

void ACharactorCard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));

    if (SLGameMode->GetPlayState() != GamePlayState::PlayState) return;

    CurrentMoveCooldown -= DeltaTime;
    if (CurrentMoveCooldown < 0)
    {
        CurrentMoveCooldown = MoveCooldown;
        CharactorMove();
    }
}

void ACharactorCard::CharactorMove()
{
    switch (CardData.CardType)
    {
    case CardType::person:
        return;
    case CardType::netural:
        Push();
        break;
    case CardType::enemy:
        PushTowardPeople();
        break;
    default:
        break;
    }
}

void ACharactorCard::SendMovementToStack(ECardMovement Movement)
{
    if (CardData.CardType == CardType::person)
    {
        Super::SendMovementToStack(Movement);
    }
}

bool ACharactorCard::Eat(TObjectPtr<ACard> Food, FCardAnimationCallback& Callback)
{
    if (CardData.CardType != CardType::person) return false;

    // ��� ī���� AddTypeValue�� �䱸 �ķ����� ���
    int FoodValue = Food->GetAddTypeValue();
    int RequireFood = GetAddTypeValue();

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
    TargetCallback.BindUObject(SLGameMode, &ASLGameModeBase::EatCompleted);

    if (FoodEaten >= RequireFood)
    {
        TargetCallback.ExecuteIfBound();
        return true;
    }

    FoodEaten += Food->GetAddTypeValue();
    Food->SetAddTypeValue(Food->GetAddTypeValue() - RequireFood);
    Food->MoveToAnother(this, TargetCallback);
    return true;

}

void ACharactorCard::PushTowardPeople()
{
    float MinDistance = 999999.9f;
    ACard* NearestPerson = nullptr;

    ASLGameModeBase* SLGameMode = Cast<ASLGameModeBase>(UGameplayStatics::GetGameMode(this));
    TArray<ACardStack*> AllCardStacks = SLGameMode->GetAllCardStacks();
    TArray<ACard*> Cards;

    for (ACardStack* CurrentCardStack : AllCardStacks)
    {
        Cards = CurrentCardStack->GetCardsByType(CardType::person);

        for (ACard* Card : Cards)
        {
            float Distance = GetDistanceTo(Card);
            if (Distance < MinDistance)
            {
                NearestPerson = Card;
                MinDistance = Distance;
            }
        }
    }
    
    if (NearestPerson == nullptr) return;

    FVector MoveVector = (NearestPerson->GetActorLocation() - GetActorLocation());
    MoveVector.Normalize();
    MoveVector *= PushVector.Size();
    MoveVector.Z = PushVector.Z;
    Push(MoveVector, true);
}