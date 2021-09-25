// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DummyPawn.generated.h"

UCLASS()
class TEMPCAPSTONEPROJECT_API ADummyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADummyPawn();

		void SetupDummyPawn(class ATempCapstoneProjectCharacter* C);

	UFUNCTION(NetMulticast, Reliable)
		void NMC_SetupDummyPawn(ATempCapstoneProjectCharacter* C);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ATempCapstoneProjectCharacter* CharToFollow;
	UPROPERTY(Replicated, BlueprintReadWrite)
	class UCameraComponent* CamToFollow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* DummyCamera;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
