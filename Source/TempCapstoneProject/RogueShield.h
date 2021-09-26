// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueShield.generated.h"

UCLASS()
class TEMPCAPSTONEPROJECT_API ARogueShield : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARogueShield();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
		class UBoxComponent* ShieldCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals", Replicated)
		class UStaticMeshComponent* ShieldMesh;

	//bool m_IsVisible = false;

	void ShieldEnable(bool e);

	UFUNCTION(Server, Reliable)
		void Server_ShieldEnable(bool e);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};