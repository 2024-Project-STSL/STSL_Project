// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainActor.generated.h"

UENUM(BlueprintType)
enum class EMouseHitObject : uint8
{
	None = 0,
	Floor,
	Card
};

UCLASS()
class STSL_API AMainActor : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EMouseHitObject MouseDraggingType;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> MouseHoveringActor;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> MouseDraggingActor;

public:

	// Sets default values for this pawn's properties
	AMainActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetMouseHoveringActor(AActor* NewMouseHoveringActor) { MouseHoveringActor = NewMouseHoveringActor; }

	UFUNCTION(BlueprintCallable)
	void SetMouseDraggingActor(AActor* NewMouseDraggingActor);

	UFUNCTION(BlueprintCallable)
	void SetMouseDraggingType(EMouseHitObject NewMouseDraggingType) { MouseDraggingType = NewMouseDraggingType; }

	UFUNCTION(BlueprintCallable)
	AActor* GetMouseHoveringActor() const { return MouseHoveringActor; }

	UFUNCTION(BlueprintCallable)
	AActor* GetMouseDraggingActor() const { return MouseDraggingActor; }

	UFUNCTION(BlueprintCallable)
	EMouseHitObject GetMouseDraggingType() const { return MouseDraggingType; }
};
