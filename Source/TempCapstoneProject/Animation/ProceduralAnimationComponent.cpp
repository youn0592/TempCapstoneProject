// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralAnimationComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PoseableMeshComponent.h"

//#include "Components/SkeletalMeshComponent.h"

UProceduralAnimationComponent::UProceduralAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelMesh"));
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
}

void UProceduralAnimationComponent::Setup(UCapsuleComponent* CharacterCapsule)
{
	CapsuleCollider = CharacterCapsule;

	// Hook up the body mesh to the capsule collider
	BodyMesh->SetupAttachment(CapsuleCollider);

	// For debugging animation
	WheelMesh->SetRelativeScale3D(FVector(AnimInfo.StrideLength, 1, AnimInfo.StrideLength));
	WheelMesh->SetRelativeLocationAndRotation(CapsuleCollider->GetRelativeLocation() + FVector(0, 0, AnimInfo.StrideLength * 100 * 0.5f - CapsuleCollider->GetScaledCapsuleHalfHeight()), CapsuleCollider->GetRelativeRotation());
	WheelMesh->SetupAttachment(CapsuleCollider);
}


void UProceduralAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UProceduralAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	WheelMesh->SetRelativeLocation(FVector(0, 0, AnimInfo.StrideLength * 100 * 0.5f - CapsuleCollider->GetScaledCapsuleHalfHeight()));
	WheelMesh->SetRelativeScale3D(FVector(AnimInfo.StrideLength, 1, AnimInfo.StrideLength));

	FVector vel = CapsuleCollider->GetComponentVelocity();
	vel.Z = 0;

	float WheelRotationThisFrame = -vel.Size() * DeltaTime / (2.5 * AnimInfo.StrideLength);
	WheelMesh->AddLocalRotation(FRotator(WheelRotationThisFrame, 0, 0));

}

