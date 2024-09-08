// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MouseInputInterface.h"
#include "BuySellInterface.h"
#include <Components/TextRenderComponent.h>
#include <Components/WidgetComponent.h>
#include "Data/CardData.h"
#include "Data/CharacterData.h"
#include "Engine/Font.h"
#include "Card.generated.h"

DECLARE_DELEGATE(FCardAnimationCallback)

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
	class UDataTable* CardDataTable;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UMaterialInstance> FoodCardMat;

	UFont* CardFont;
	UMaterial* CardFontMat;

	// 조합 진행 바
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* CraftingProgressWidget;

	UPROPERTY(VisibleAnywhere)
	FVector CardOffset;

	UPROPERTY(EditDefaultsOnly)
	float CardMass = 10.0f;

	UPROPERTY(VisibleAnywhere)
	bool bShowProgressBar = false;

	TMap<FString, float> WorldBorder;
	TMap<FString, float> WorldBorderWithoutBuyArea;

	bool bPhysicsBeforeBreak = false;

protected:
	UPROPERTY(VisibleAnywhere)
	bool bFloating = false;

	// 제작 완료된 카드가 날아갈 힘 - ±(X/2~X), ±(Y/2~Y), 0~Z
	UPROPERTY(EditDefaultsOnly)
	FVector PushVector = FVector(6000.0f, 6000.0f, 3000.0f);

	UPROPERTY(VisibleAnywhere)
	AActor* CardStack;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* VisualMesh;

	UPROPERTY(VisibleAnywhere)
	FCardData CardData;

	// 카드 제목 텍스트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* TitleText;

	// 판매가
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* SellPriceText;

	// 추가 기능
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* AddTypeText;

	// 가격 아이콘
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* PriceIcon;

	// 체력 아이콘 (생명체)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* HealthIcon;

	// 카드 이미지
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* CardImageWidget;

	UPROPERTY(EditDefaultsOnly)
	float FontSize = 80.0f;

	// Push 이후 처음으로 땅에 떨어질 때까지 드래깅 방지
	UPROPERTY(VisibleAnywhere)
	bool bPreventDragging = false;

	// 카드 강제 이동 시 목표 위치
	FVector TargetLocation = FVector::ZeroVector;
	FVector MovedLocation;

	UPROPERTY(EditDefaultsOnly)
	float TargetFollowSpeed = 5.0f;

public:	
	// Sets default values for this actor's properties
	ACard();
	ACard(int32 CardID);

	UFUNCTION(BlueprintCallable)
	void SetFloating(bool NewFloating) { bFloating = NewFloating; }

	UFUNCTION(BlueprintCallable, Category = "CardID")
	int32 GetCardID() const { return CardData.CardCode; }

	UFUNCTION(BlueprintCallable, Category = "CardID")
	virtual void SetCardID(int32 NewCardID) { CardData.CardCode = NewCardID; LoadCard(); }

	UFUNCTION(BlueprintCallable)
	void SetShowProgressBar(bool NewShowProgressBar);

	UFUNCTION(BlueprintCallable)
	CardType GetCardType() const { return CardData.CardType; }

	UFUNCTION(BlueprintCallable)
	AddType GetAddType() const { return CardData.AddType; }

	UFUNCTION(BlueprintCallable)
	void SetAddTypeValue(int Value);

	UFUNCTION(BlueprintCallable)
	int GetAddTypeValue() const { return CardData.AddTypeValue; }

	UFUNCTION(BlueprintCallable)
	int GetCardPrice() const { return CardData.CardPrice; }

	UFUNCTION(BlueprintCallable)
	void ResetTargetLocation() { TargetLocation = FVector::Zero(); }

	FCardAnimationCallback TargetCallback;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitCompoent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void LoadCard();

	void SetCardStack(AActor* Stack);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	AActor* GetCardStack() const { return CardStack; }

	UStaticMeshComponent* GetVisualMesh() const { return VisualMesh; }

	bool IsCharacter() const { return CardData.IsCharacter(); }

	// 마우스 카드 이동 관련
	virtual void SendMovementToStack(ECardMovement Movement);
	FVector GetMouseHitLocation() const;

	virtual void StartMouseHover() override;
	void StartHover(float HoveringHeight);

	virtual void EndMouseHover() override;
	void EndHover();

	virtual void StartDrag() override;
	virtual void StartCardDrag();

	virtual void EndDrag() override;
	virtual void EndCardDrag();

	virtual void MoveToCursor() override;
	void MoveCardToCursor(float FloatingHeight);

	// 벡터만큼 카드 밀어내기
	void Push(FVector Force, bool bPrecise = false);
	void Push();

	// 순간이동
	void Move(FVector Movement);

	void UpdateProgressBar(float Current);
	void UpdateProgressBar(float Current, float Max);

	// 땅에 닿은 카드(스택)의 위치 재조정
	void UpdateGroundPosition();

	void UpdateWorldBorder(int Length);
	void ResetWorldBorder();

	void BreakGame();
	void ResumeGame();

	void Remove();

	void MoveBack(FCardAnimationCallback& Callback);

	void MoveToAnother(ACard* OtherCard, FCardAnimationCallback& Callback);
	void MoveToAnother(ACard* OtherCard, FCardAnimationCallback& Callback, float Distance);
};
