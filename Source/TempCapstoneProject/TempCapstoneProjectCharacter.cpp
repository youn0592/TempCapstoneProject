// Copyright Epic Games, Inc. All Rights Reserved.

#include "TempCapstoneProjectCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "TempCapstoneProjectGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "DummyPawn.h"

//////////////////////////////////////////////////////////////////////////
// ATempCapstoneProjectCharacter

ATempCapstoneProjectCharacter::ATempCapstoneProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	static ConstructorHelpers::FClassFinder<APawn> DummyLocalBP_Getter(TEXT("/Game/Blueprints/Characters/BP_DummyPawn"));

	if (DummyLocalBP_Getter.Class != NULL)
	{
		pDummyBP = DummyLocalBP_Getter.Class;
	}

	//	DummyController = CreateDefaultSubobject<APlayerController>(TEXT("dummyController"));

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//	UGameplayStatics::CreatePlayer(GetWorld(), -1);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//	void ATempCapstoneProjectCharacter::SpawnDummy(APlayerController* dummyController)
//	{
//	}

void ATempCapstoneProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// RemoteRole = ROLE_SimulatedProxy
	int local = GetLocalRole();
	int remote = GetRemoteRole();

	//	if (GetLocalRole() == ROLE_AutonomousProxy)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 50, FColor::White, GetName());
	//	}
	// GEngine->AddOnScreenDebugMessage(-1, 50, FColor::White, FString::Printf(TEXT("%d , %d"), local, remote));
	// GEngine->AddOnScreenDebugMessage(-1, 50, FColor::White, GetName());
	//	switch (GetLocalRole())
	//	{
	//	case ROLE_None:
	//		GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Red, FString::Printf(TEXT("NONE |")));
	//		break;
	//	case ROLE_SimulatedProxy:
	//		GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Green, FString::Printf(TEXT("SimProc |")));
	//		break;
	//	case ROLE_AutonomousProxy:
	//		GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Blue, FString::Printf(TEXT("AutProc |")));
	//		break;
	//	case ROLE_Authority:
	//		GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Yellow, FString::Printf(TEXT("Auth |")));
	//		break;
	//	}

		if(GetController())
		{
			UGameplayStatics::CreatePlayer(GetWorld(), -1);
		}

}


void ATempCapstoneProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATempCapstoneProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATempCapstoneProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATempCapstoneProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATempCapstoneProjectCharacter::LookUpAtRate);

	// PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ATempCapstoneProjectCharacter::Dash);
}

void ATempCapstoneProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATempCapstoneProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATempCapstoneProjectCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATempCapstoneProjectCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
