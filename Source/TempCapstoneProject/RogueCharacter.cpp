// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/Character.h"

void ARogueCharacter::Dash()
{
	if (m_CanDash == false)
		return;

	m_CanDash = false;

	// Launches the character in forward direction.
	LaunchCharacter((DashDistance / DashTime) * GetCapsuleComponent()->GetForwardVector(), false, false);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]
		{
			GetMovementComponent()->StopMovementImmediately();
		});

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DashTime, false);

	FTimerDelegate TimerDelegate2;
	TimerDelegate2.BindLambda([&]
		{
			m_CanDash = true;
		});

	FTimerHandle TimerHandle2;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, TimerDelegate2, DashReset, false);
}

void ARogueCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ARogueCharacter::Dash);
}