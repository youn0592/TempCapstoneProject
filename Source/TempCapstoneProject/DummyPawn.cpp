// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyPawn.h"
#include "TempCapstoneProjectCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ADummyPawn::ADummyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DummyCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DummyCamera"));

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ADummyPawn::BeginPlay()
{
	Super::BeginPlay();


}

void ADummyPawn::SetupDummyPawn(ATempCapstoneProjectCharacter* C)
{
	if (HasAuthority())
	{
		NMC_SetupDummyPawn_Implementation(C);
	}
}

// Called when the game starts or when spawned
void ADummyPawn::NMC_SetupDummyPawn_Implementation(ATempCapstoneProjectCharacter* C)
{
	CamToFollow = C->GetFollowCamera();
}

// Called every frame
void ADummyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CamToFollow)
	{
		FVector targetCamPos = CamToFollow->GetComponentLocation();
		FRotator targetCamRot = CamToFollow->GetComponentRotation();
		
		DummyCamera->SetRelativeLocation(FMath::VInterpTo(DummyCamera->GetRelativeLocation(), targetCamPos, DeltaTime, 20.f));
		DummyCamera->SetRelativeRotation(FMath::RInterpTo(DummyCamera->GetRelativeRotation(), targetCamRot, DeltaTime, 20.f));
	}
}

// Called to bind functionality to input
void ADummyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

}


void ADummyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADummyPawn, CamToFollow);
}
