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

	// ������ �о ���Ϳ� ���� ���
	UPROPERTY(EditDefaultsOnly)
	float OtherCollsionWeight = -1.0f;

	// �� ī�� ID �� ī���� ����
	UPROPERTY(VisibleAnywhere, Category = "CardStack")
	TMap<int32, int32> CardCount;

	// ���� ���� ������ ��ȣ, -1�� ��� ���� 
	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	int CraftingRecipeID = -1;

	// ī�� ���ۿ� �ɸ��� �ð�
	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	float MakeTime = 5.0f;

	// ���� ī�� ���� ���൵
	UPROPERTY(VisibleAnywhere, Category = "Crafting")
	float CraftingProgress = 0.0f;

	// ����� ī���� �ֺ� ���� �˻� ����
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

	// ī�� �浹 ����
	void GetCardCollisionVector(AActor* Other, FVector& SelfVector, FVector& OtherVector) const;
	void PushCards(FVector Force);

	// ��� ���ÿ� �� ������ ���� �� �ִ°�?
	static bool GetCardStackable(ACardStack* CardStack, ACardStack* OtherStack);
	
	// ExceptionStack: �˻� ���� ����, ����� ī�忡���� �������� ������ ���
	ACardStack* GetNearestStackable(float StackSearchDistance, ACardStack* ExceptionStack = nullptr);

	// ���� index ������ ������ ������ ���ο� �������� ����
	static void SplitCardStack(ACardStack* CardStack, int32 Index);

	// ���� ���� ���� �ٸ� ������ ���ΰ�?
	UFUNCTION(BlueprintCallable)
	void SetShowProgressBar(bool NewShowProgressBar) const;

	// �ڽ��� ��ġ�� ù ī���� ��ġ�� �ݿ��ϰ�
	// ���� ������ ���� ���ݾ� Ʋ������ ī�� ��ġ ����
	void UpdatePosition(bool bFalling = false);
};
