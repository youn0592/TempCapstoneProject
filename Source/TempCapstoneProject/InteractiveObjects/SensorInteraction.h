// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../InteractionInterface.h"
#include "SensorInteraction.generated.h"

UCLASS()
class TEMPCAPSTONEPROJECT_API ASensorInteraction : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASensorInteraction();

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

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsActive() { return bIsActive; };

private:
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* SensorCollider;

	UPROPERTY(EditAnywhere, Category = "Interactive Object")
	class UStaticMeshComponent* ObjectMesh;

	UPROPERTY(EditAnywhere, Category = "Interactive Object")
	class UWidgetComponent* InteractionWidget;
};
