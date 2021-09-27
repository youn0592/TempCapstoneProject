// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/Character.h"
#include "TempCapstoneProject.h"
#include "Net/UnrealNetwork.h"

ARogueCharacter::ARogueCharacter()
{
	SetReplicates(true);
}

void ARogueCharacter::Dash()
{
	Server_Dash();
}

void ARogueCharacter::Server_Dash_Implementation()
{
	if (m_CanDash == false)
		return;

	m_CanDash = false;
	m_InitialVel = GetCharacterMovement()->Velocity;
	m_GroundFriction = GetCharacterMovement()->GroundFriction;

	// Launches the character in forward direction.
	GetCharacterMovement()->GroundFriction = 0.f; // Remove Friction so that Distance in air = Distance on ground during dash.
	LaunchCharacter((DashDistance / DashTime) * GetCapsuleComponent()->GetForwardVector(), false, false);

	FTimerDelegate EndDashDelegate;
	EndDashDelegate.BindLambda([&] // Gets called after LaunchCharacter
	{
		GetMovementComponent()->StopMovementImmediately();
		GetMovementComponent()->Velocity = m_InitialVel;
		GetCharacterMovement()->GroundFriction = m_GroundFriction;
	});

	FTimerHandle DashDurationHandle;
	GetWorld()->GetTimerManager().SetTimer(DashDurationHandle, EndDashDelegate, DashTime, false);

	FTimerDelegate DashResetDelegate; // Gets called after Dash
	DashResetDelegate.BindLambda([&]
	{
		m_CanDash = true;
	});

	FTimerHandle DashResetHandle;
	GetWorld()->GetTimerManager().SetTimer(DashResetHandle, DashResetDelegate, DashReset, false);
}

void ARogueCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ARogueCharacter::Dash);
}

bool ARogueCharacter::GetIsDashing()
{
	return !m_CanDash;
}

ECharacterType ARogueCharacter::GetCharacterType()
{
	return ECharacterType::Rogue;
}