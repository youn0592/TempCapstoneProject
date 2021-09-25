// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../InteractionInterface.h"
#include "InteractiveActor.generated.h"

UCLASS()
class TEMPCAPSTONEPROJECT_API AInteractiveActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractiveActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Object")
    AActor* TargetActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Custom Functions
	virtual void Interact() override;
	virtual void ShowInteractionWidget() override;
	virtual void HideInteractionWidget() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ReceiveInteract"))
	void ReceiveInteract();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetTargetActor() { return TargetActor; };

protected:

	bool bIsActive = false;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsActive() { return bIsActive; };

private:
	UPROPERTY(EditAnywhere)
	class USceneComponent* _RootComponent;

	UPROPERTY(EditAnywhere, Category = "Interactive Object")
	class UStaticMeshComponent* ObjectMesh;

	UPROPERTY(EditAnywhere, Category = "Interactive Object")
    class UWidgetComponent* InteractionWidget;
};
