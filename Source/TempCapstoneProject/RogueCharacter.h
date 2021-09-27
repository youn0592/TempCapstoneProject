// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TempCapstoneProjectCharacter.h"
#include "RogueCharacter.generated.h"

/**
 * 
 */
UCLASS()
class TEMPCAPSTONEPROJECT_API ARogueCharacter : public ATempCapstoneProjectCharacter
{
	GENERATED_BODY()
	
public:
	ARogueCharacter();

	UPROPERTY(EditAnywhere)
		float DashDistance;

	UPROPERTY(EditAnywhere)
		float DashTime;

	UPROPERTY(EditAnywhere)
		float DashReset;

	void Dash();
	
	UFUNCTION(Server, Reliable)
		void Server_Dash();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield", Replicated)
		class ARogueShield* RogueShield;

	UPROPERTY(Replicated)
		bool m_IsShieldVisible = false;

	UPROPERTY(Replicated)
		bool m_CanDash = true;

protected:
	virtual void BeginPlay() override;

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//void SpawnShield();

	/*UFUNCTION(NetMulticaast, Reliable)
		void NMC_SpawnShield();*/

	void ShieldPlatform();

	UFUNCTION(Server, Reliable)
		void Server_ShieldPlatform();
private:

	virtual void Tick(float DeltaTime) override;



	FVector m_InitialVel;
	float m_GroundFriction;
};
