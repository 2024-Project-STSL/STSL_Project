// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Card.h"
#include "MouseInputInterface.h"
#include <Components/BoxComponent.h>
#include "Data/RecipeData.h"
#include "Data/DropData.h"
#include "CardStack.generated.h"

UCLASS()
class STSL_API ACardStack : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "CardStack")
	TArray<AActor*> Cards;

	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	UDataTable* CraftingRecipeTable;

	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	UDataTable* DropTable;

	// X Offset between cards
	UPROPERTY(EditDefaultsOnly, Category = "CardStack")
	float XOffset = -100.0f;

	// Z Offset between cards
	UPROPERTY(EditDefaultsOnly, Category = "CardStack")
	float ZOffset = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float HoveringHeight = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float FloatingHeight = 40.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float HeightOffset = 0.4f;

	UPROPERTY(EditDefaultsOnly)
	float Collsionforce = 50.0f;

	// 상대방을 밀어낼 벡터에 곱할 계수
	UPROPERTY(EditDefaultsOnly)
	float OtherCollsionWeight = -1.0f;

	// 각 카드 ID 별 카드의 개수
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TMap<int32, int32> CardCount;

	// 제작 중인 레시피 번호, -1일 경우 없음 
	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	int CraftingRecipeID = -1;

	// 카드 제작에 걸리는 시간
	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	float MakeTime = 5.0f;

	// 현재 카드 제작 진행도
	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	float CraftingProgress = 0.0f;

	// 생산된 카드의 주변 스택 검색 범위
	UPROPERTY(EditAnywhere, Category = "CardStack")
	float SearchDistance = 500.0f;

public:	
	// Sets default values for this actor's properties
	ACardStack();

protected:
	USceneComponent* SceneComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void RemoveFromGamemode();

	bool CheckCraftingRecipe(FRecipeData *Recipe);

	bool CheckDropRecipe(FDropData* Recipe);

	void UpdateCraftingRecipe();

	void CompleteCrafting();

	void CompleteProducing();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Add a card to the stack
	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void AddCard(AActor* Card);
	void AddCard(TArray<AActor*> NewCards);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	void RemoveCard(int32 Index, bool bDespawn = false);
	void RemoveCard(AActor* Card, bool bDespawn = false);
	void RemoveCard(TArray<AActor*> NewCards, bool bDespawn = false);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	ACard* GetFirstCard() const { return Cast<ACard>(Cards[0]); }
	UFUNCTION(BlueprintCallable, Category = "CardStack")
	ACard* GetLastCard() const { return Cast<ACard>(Cards.Last()); }

	void HandleStackMove(ACard* Sender, ECardMovement Movement);

	UFUNCTION(BlueprintCallable, Category = "CardStack")
	AActor* FindMouseSender(FVector Location) const;

	void HandleStackCollision(ACard* OtherCard);

	// 카드 충돌 관련
	void GetCardCollisionVector(AActor* Other, FVector& SelfVector, FVector& OtherVector) const;
	void PushCards(FVector Force);

	// 상대 스택에 이 스택이 쌓일 수 있는가?
	static bool GetCardStackable(ACardStack* CardStack, ACardStack* OtherStack);
	
	// ExceptionStack: 검색 예외 스택, 생산된 카드에서는 생산지를 지정해 사용
	ACardStack* GetNearestStackable(float StackSearchDistance, ACardStack* ExceptionStack = nullptr);

	// 기준 index 이전의 스택을 나누어 새로운 스택으로 만듦
	static void SplitCardStack(ACardStack* CardStack, int32 Index);

	// 스택 위에 진행 바를 보여줄 것인가?
	UFUNCTION(BlueprintCallable)
	void SetShowProgressBar(bool NewShowProgressBar) const;

	// 자신의 위치를 첫 카드의 위치로 반영하고
	// 물리 엔진에 의해 조금씩 틀어지는 카드 위치 보정
	void UpdatePosition(bool bFalling = false);
};
