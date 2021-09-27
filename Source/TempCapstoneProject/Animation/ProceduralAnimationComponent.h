// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralAnimationInfo.h"
#include "ProceduralAnimationComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEMPCAPSTONEPROJECT_API UProceduralAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	enum class FootEnum { LeftFoot, RightFoot };

	UProceduralAnimationComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		FProceduralAnimationInfo AnimInfo;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* WheelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float MinStrideLength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float MaxStrideLength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float StepWidth = 1.0f;
	
	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
	//		float HipWidth = 1.0f;
	
	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
	//		float LegLength = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float LeanScale = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float LeanLimit = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float LeanOvershootScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float JumpLeanScale = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float IK_PredictionRate = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float IK_ReplantRate = 3.0f;

protected:

	UPROPERTY(Replicated)
	class UCapsuleComponent* CapsuleCollider = nullptr;
	UPROPERTY(Replicated)
	class UCharacterMovementComponent* MovementComp = nullptr;

	// Lean variables
	FVector VelocityChange = FVector::ZeroVector;
	FVector OldVelocity = FVector::ZeroVector;
	FVector LeanOvershoot = FVector::ZeroVector;

	// IK variables
	class USkeletalMeshComponent* CharacterMesh;

	float IK_SquareDistanceSinceReplant = 0;
	
	float IK_TriggerFactor = 0;
	bool IK_MustReplant = true;
	FTransform IK_LastL;
	FTransform IK_NextL;
	FTransform IK_LastR;
	FTransform IK_NextR;

	float previousCalcZ = 0;

	FootEnum IK_ReachingFoot = FootEnum::LeftFoot;
	FVector IK_PredictedCapsuleLocation;

	float CalcPelvisZ = 0;
public:	
	
	UFUNCTION(BlueprintCallable)
		FProceduralAnimationInfo GetAnimInfo();

	void HandleIK(float DeltaTime);

	void Setup();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	void HandleHamsterWheel(float DeltaTime, FVector worldVelocity);
	void HandleLean(float DeltaTime, FVector worldVelocity);



	void UpdateIKTarget();

};
