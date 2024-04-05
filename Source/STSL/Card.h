// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MouseInputInterface.h"
#include <Components/TextRenderComponent.h>
#include <Components/WidgetComponent.h>
#include "Data/CardData.h"
#include "Engine/Font.h"
#include "Card.generated.h"

UENUM(BlueprintType)
enum class ECardMovement : uint8
{
	StartHover = 0,
	EndHover,
	StartDrag,
	EndDrag,
	MoveToCursor
};

UCLASS()
class STSL_API ACard : public AActor, public IMouseInputInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	AActor* CardStack;

	UPROPERTY(VisibleAnywhere)
	class UDataTable* CardDataTable;

	UPROPERTY(VisibleAnywhere)
	FCardData CardData;

	UFont* CardFont;
	UMaterial* CardFontMat;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* VisualMesh;

	// ī�� ���� �ؽ�Ʈ
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* TitleText;

	// �ǸŰ�
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* SellPriceText;

	// �߰� ���
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* AddTypeText;

	// ī�� �̹���
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* CardImageWidget;

	// ���� ���� ��
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* CraftingProgressWidget;

	UPROPERTY(VisibleAnywhere)
	FVector CardOffset;

	UPROPERTY(EditDefaultsOnly)
	float CardMass = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float FontSize = 80.0f;

public:	
	// Sets default values for this actor's properties
	ACard();
	ACard(int32 CardID);

	UFUNCTION(BlueprintCallable, Category = "CardID")
	int32 GetCardID() const { return CardData.CardCode; }

	UFUNCTION(BlueprintCallable, Category = "CardID")
	void SetCardID(int32 NewCardID) { CardData.CardCode = NewCardID; LoadCard(); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitCompoent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void LoadCard();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetCardStack(AActor* Stack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	AActor* GetCardStack() const { return CardStack; }

	UStaticMeshComponent* GetVisualMesh() const { return VisualMesh; }

	// ���콺 ī�� �̵� ����
	void SendMovementToStack(ECardMovement Movement);
	FVector GetMouseHitLocation() const;

	virtual void StartMouseHover() override;
	void StartHover(float HoveringHeight);

	virtual void EndMouseHover() override;
	void EndHover();

	virtual void StartDrag() override;
	void StartCardDrag();

	virtual void EndDrag() override;
	void EndCardDrag();

	virtual void MoveToCursor() override;
	void MoveCardToCursor(float FloatingHeight);

	// ���͸�ŭ ī�� �о��
	void Push(FVector Force);
};
