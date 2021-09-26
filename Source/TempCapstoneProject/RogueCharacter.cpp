// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "RogueShield.h"

ARogueCharacter::ARogueCharacter()
{
	SetReplicates(true);
}

void ARogueCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Create the shield
	RogueShield = GetWorld()->SpawnActor<ARogueShield>(GetCapsuleComponent()->GetRelativeLocation() + FVector(0.0f, 0.0f, 100.0f), GetCapsuleComponent()->GetRelativeRotation());
	RogueShield->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
}

//void ARogueCharacter::SpawnShield()
//{
//
//}

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

	// Input for Holding the Shield up
	PlayerInputComponent->BindAction("ShieldPlatform", IE_Pressed, this, &ARogueCharacter::ShieldPlatform);
	//PlayerInputComponent->BindAction("ShieldPlatform", IE_Released, this, &ARogueCharacter::ShieldPlatform);
}

void ARogueCharacter::ShieldPlatform()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("%d"), m_IsShieldVisible));
	Server_ShieldPlatform();
	m_IsShieldVisible = !m_IsShieldVisible;

	/*if (RogueShield)
		RogueShield->ShieldEnable(m_IsShieldVisible);*/

}

void ARogueCharacter::Server_ShieldPlatform_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("%d"), m_IsShieldVisible));

	m_IsShieldVisible = !m_IsShieldVisible;
	if (RogueShield)
		RogueShield->ShieldEnable(m_IsShieldVisible);
}

void ARogueCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARogueCharacter, RogueShield);
	DOREPLIFETIME(ARogueCharacter, m_IsShieldVisible);
}