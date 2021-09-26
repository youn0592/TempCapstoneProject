// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../InteractionInterface.h"
#include "ButtonInteraction.generated.h"

UCLASS()
class TEMPCAPSTONEPROJECT_API AButtonInteraction : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButtonInteraction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Object")
	AActor* TargetActor;

protected:

	bool bIsActive = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Interface Functions
	virtual void Interact() override;
	virtual void ShowInteractionWidget() override;
	virtual void HideInteractionWidget() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ReceiveInteract"))
	void ReceiveInteract();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetTargetActor() { return TargetActor; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsActive() { return bIsActive; };

private:
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, Category = "Interactive Object")
	class UBoxComponent* ButtonCollider;

	UPROPERTY(EditAnywhere, Category = "Interactive Object")
	class USkeletalMeshComponent* ObjectMesh;

	UPROPERTY(EditAnywhere, Category = "Interactive Object")
	class UWidgetComponent* InteractionWidget;
};
