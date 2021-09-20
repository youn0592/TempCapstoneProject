// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralAnimationComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
// #include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UProceduralAnimationComponent::UProceduralAnimationComponent() : AnimInfo()
{
	PrimaryComponentTick.bCanEverTick = true;
	WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelMesh"));
	//IK_SquareDistanceSinceReplant = 0;
	//BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
}

void UProceduralAnimationComponent::Setup()
{
	ACharacter* C = Cast<ACharacter>(GetOwner());
	
	CapsuleCollider = C->GetCapsuleComponent();
	MovementComp = C->GetCharacterMovement();

	// For debugging animation
	WheelMesh->SetRelativeScale3D(FVector(MaxStrideLength, 1, MaxStrideLength));
	WheelMesh->SetRelativeLocationAndRotation(FVector(0, 0, MaxStrideLength * 100 * 0.5f - CapsuleCollider->GetScaledCapsuleHalfHeight()), CapsuleCollider->GetRelativeRotation());
	WheelMesh->AttachToComponent(CapsuleCollider, FAttachmentTransformRules::KeepRelativeTransform);

	FVector CapsuleBase = CapsuleCollider->GetRelativeLocation() - FVector(0, 0, CapsuleCollider->GetScaledCapsuleHalfHeight());
	FVector LegOffset_Right = CapsuleCollider->GetRightVector() * StepWidth;

	IK_LastL.SetLocation(CapsuleBase - LegOffset_Right);
	IK_LastL.SetRotation(CapsuleCollider->GetRelativeRotation().Quaternion());
	IK_LastR.SetLocation(CapsuleBase + LegOffset_Right);
	IK_LastR.SetRotation(CapsuleCollider->GetRelativeRotation().Quaternion());

	IK_NextL = IK_LastL;
	IK_NextL = IK_LastR;

	IK_PredictedCapsuleLocation = CapsuleBase;
}

//	void UProceduralAnimationComponent::BeginPlay()
//	{
//		Super::BeginPlay();
//	}

FProceduralAnimationInfo UProceduralAnimationComponent::GetAnimInfo()
{
	return AnimInfo;
}

void UProceduralAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (CapsuleCollider == nullptr)
		return;
	

	FVector Worldvel = CapsuleCollider->GetComponentVelocity();
	HandleHamsterWheel(DeltaTime, Worldvel);
	HandleLean(DeltaTime, Worldvel);
	HandleIK(DeltaTime);

}

void UProceduralAnimationComponent::HandleHamsterWheel(float DeltaTime, FVector worldVelocity)
{
	WheelMesh->SetRelativeLocation(FVector(0, 0, MaxStrideLength * 100 * 0.5f - CapsuleCollider->GetScaledCapsuleHalfHeight()));
	WheelMesh->SetRelativeScale3D(FVector(MaxStrideLength, 1, MaxStrideLength));

	FVector WorldVel2D = worldVelocity;
	WorldVel2D.Z = 0;

	float WheelRotationThisFrame = WorldVel2D.Size() * 0.01f * DeltaTime * 180.f / (MaxStrideLength * 0.5f * PI);
	float PercentAroundWheelThisFrame = WorldVel2D.Size() * 0.01f * DeltaTime / (2 * PI * MaxStrideLength * 0.5f);

	AnimInfo.PoseBlendAlpha += PercentAroundWheelThisFrame;
	
	IK_TriggerFactor += 2 * PercentAroundWheelThisFrame;
	
	if (IK_TriggerFactor > 0.5f)
	{
		IK_MustReplant = true;
		IK_TriggerFactor = 0.f;
	}

	AnimInfo.RFootIK_BlendFactor = FMath::Clamp(sinf(2 * PI * AnimInfo.PoseBlendAlpha), 0.f, 1.f);
	AnimInfo.LFootIK_BlendFactor = FMath::Clamp(sinf(PI * (2 * AnimInfo.PoseBlendAlpha + 0.5f)), 0.f, 1.f);

	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("%f"), AnimInfo.LFootIK_BlendFactor));
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("%f"), AnimInfo.RFootIK_BlendFactor));

	AnimInfo.PoseBlendAlpha = fmod(AnimInfo.PoseBlendAlpha, 1.0f);

	WheelMesh->AddLocalRotation(FRotator(-WheelRotationThisFrame, 0, 0));
}

void UProceduralAnimationComponent::HandleLean(float DeltaTime, FVector worldVelocity)
{

//--Calculate Velocity change from last frame. Store info as "AccelVector" by incorporating the amount of time passed
	VelocityChange = worldVelocity - OldVelocity;
	VelocityChange = FVector(
		FVector::DotProduct(VelocityChange, CapsuleCollider->GetForwardVector()),
		FVector::DotProduct(VelocityChange, CapsuleCollider->GetRightVector()),
		VelocityChange.Z
	);

	FVector AccelVector = 50 * VelocityChange * DeltaTime;

	OldVelocity = worldVelocity;
//--

//--Rescale the amount of forward leaning that occurs going forward vs backwards.
	float XLeanScale = AccelVector.X < 0 ? 1.f : 2.f;
	AccelVector.X *= XLeanScale;
//--

//--Lerp LeanOvershoot towards the AccelVector. Faster forwards leaning correction if we're slowing down, slower if we're speeding up (RECONSIDER?) (TEST FOR FRAME DEPENDANCY ISSUES)
	float OvershootFactorCoefficient = 2.f;

	LeanOvershoot.X = FMath::Lerp(LeanOvershoot.X, AccelVector.X, FMath::Clamp(XLeanScale * OvershootFactorCoefficient * DeltaTime, 0.f, 1.f));
	LeanOvershoot.Y = FMath::Lerp(LeanOvershoot.Y, AccelVector.Y, FMath::Clamp(OvershootFactorCoefficient * DeltaTime, 0.f, 1.f));
	LeanOvershoot.Z = FMath::Lerp(LeanOvershoot.Z, AccelVector.Z, FMath::Clamp(OvershootFactorCoefficient * DeltaTime, 0.f, 1.f));

//--Play with this to scale leaning caused by jumping.
	float FinJumpLeanScale = AccelVector.Z < 0 ? 1.f : 1.f;

//--Calculate final Lean amount by lerping to the sum of velocity-based, acceleration-based and Jump-based leaning.
//--Clamped velocity-based side-leaning and jump based leaning. (RECONSIDER CLAMPING OR EXPOSE SOME OF THOSE VALUES) (TEST FOR FRAME DEPENDANCY ISSUES)
	AnimInfo.Lean = FMath::Lerp
	(
		AnimInfo.Lean,
		LeanScale * FRotator
		(
			FVector::DotProduct(worldVelocity, CapsuleCollider->GetForwardVector()),
			FMath::Clamp(FVector::DotProduct(worldVelocity, CapsuleCollider->GetRightVector()), -LeanLimit, LeanLimit),
			0
		)
		// + FRotator(LeanOvershoot.X + LeanOvershoot.Z, 0, LeanOvershoot.Y),

		+ FRotator(LeanOvershootScale * LeanOvershoot.X + JumpLeanScale * FinJumpLeanScale * FMath::Clamp(LeanOvershoot.Z, -10.f, 10.f), LeanOvershootScale * LeanOvershoot.Y, 0),

		FMath::Clamp(10.0f * DeltaTime, 0.f, 1.f)
	);
}

void UProceduralAnimationComponent::HandleIK(float DeltaTime)
{
	FVector CapsuleBase = CapsuleCollider->GetRelativeLocation() - FVector(0, 0, CapsuleCollider->GetScaledCapsuleHalfHeight());
	IK_PredictedCapsuleLocation = CapsuleBase + CapsuleCollider->GetComponentVelocity() * IK_PredictionRate + MovementComp->GetCurrentAcceleration() * IK_PredictionRate * IK_PredictionRate;

	// IK_SquareDistanceSinceReplant += CapsuleCollider->GetComponentVelocity().Size() * 0.01f * DeltaTime + DeltaTime * 5;
	IK_SquareDistanceSinceReplant += DeltaTime * (5 / FMath::Clamp(CapsuleCollider->GetComponentVelocity().SizeSquared(),0.f,5.f));

	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("%f"), IK_SquareDistanceSinceReplant));

	//if (> IK_ReplantRate)
	if(IK_MustReplant || IK_SquareDistanceSinceReplant > IK_ReplantRate)
	{
		UpdateIKTarget();
		IK_MustReplant = false;
		GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Cyan, TEXT("REPLANT"));
		IK_SquareDistanceSinceReplant = 0;
	}

	DrawDebugSphere(GetWorld(), IK_NextL.GetLocation(), 10.0f, 8, FColor::Cyan);
	DrawDebugSphere(GetWorld(), IK_NextR.GetLocation(), 10.0f, 8, FColor::Red);

	DrawDebugSphere(GetWorld(), IK_LastL.GetLocation(), 10.0f, 8, FColor::Black);
	DrawDebugSphere(GetWorld(), IK_LastR.GetLocation(), 10.0f, 8, FColor::Black);

	DrawDebugSphere(GetWorld(), IK_PredictedCapsuleLocation, 10.0f, 8, FColor::White);
	
}

void UProceduralAnimationComponent::UpdateIKTarget()
{
	FVector targetPos = IK_PredictedCapsuleLocation;

	switch (IK_ReachingFoot)
	{
		case FootEnum::LeftFoot:
			targetPos -= CapsuleCollider->GetRightVector() * StepWidth;
			IK_LastL = IK_NextL;
			IK_NextL.SetLocation(targetPos);
			AnimInfo.LFootIK_Target = IK_NextL;
			IK_ReachingFoot = FootEnum::RightFoot;
			break;

		case FootEnum::RightFoot:
			targetPos += CapsuleCollider->GetRightVector() * StepWidth;
			IK_LastR = IK_NextR;
			IK_NextR.SetLocation(targetPos);
			AnimInfo.RFootIK_Target = IK_NextR;
			IK_ReachingFoot = FootEnum::LeftFoot;
			break;
	}





	// FVector CapsuleBase = CapsuleCollider->GetRelativeLocation() - FVector(0, 0, CapsuleCollider->GetScaledCapsuleHalfHeight());
	// FVector LegOffset_Right = CapsuleCollider->GetRightVector() * StepWidth;

	// FVector PredictedCapsuleLocation = CapsuleBase + LegOffset_Right;
	// FVector LOrigin = CapsuleBase - LegOffset_Right;

	// FVector TargetL = LOrigin + CapsuleCollider->GetComponentVelocity() * deltaTime + CapsuleCollider->GetComponentAcceler * deltaTime
}

// CODE DUMP:

//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("%f"), LeanLimit));
//		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%f,%f,%f"), worldVelocity.X, worldVelocity.Y, worldVelocity.Z));
//		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("%f,%f,%f"), AnimInfo.Lean.Pitch, AnimInfo.Lean.Yaw, AnimInfo.Lean.Roll));
//	}

//	void UProceduralAnimationComponent::UpdateIKTargets(FVector localVelocity)
//	{
//		if (localVelocity.SizeSquared() < 10)
//			return;
//		// FVector fwd = CapsuleCollider->GetForwardVector();
//		// FVector rgt = CapsuleCollider->GetRightVector();
//		
//		FVector fwd = localVelocity.GetSafeNormal();
//		FVector rgt = FVector(-fwd.Y, fwd.X, 0).GetSafeNormal();
//	
//		FVector OriginL = CapsuleCollider->GetRelativeLocation() - rgt * HipWidth;
//		FVector OriginR = CapsuleCollider->GetRelativeLocation() + rgt * HipWidth;
//		
//		// FVector FwdOffset = CapsuleCollider->GetForwardVector() * StrideWidth;
//		// FVector RgtOffset = CapsuleCollider->GetRightVector() * StrideWidth;
//	
//		float maxAngle = 30;
//	
//		float ForwardAngle = maxAngle * sin(2 * PI * AnimInfo.PoseBlendAlpha) * FVector::DotProduct(fwd, localVelocity.GetSafeNormal());
//		float SideAngle = maxAngle * sin(2 * PI * AnimInfo.PoseBlendAlpha) * FVector::DotProduct(rgt, localVelocity.GetSafeNormal());
//	
//		FVector LRayEndpoint = OriginL + LegLength * FVector::DownVector.RotateAngleAxis( ForwardAngle, rgt).RotateAngleAxis( SideAngle, fwd);
//		FVector RRayEndpoint = OriginR + LegLength * FVector::DownVector.RotateAngleAxis(-ForwardAngle, rgt).RotateAngleAxis(-SideAngle, fwd);
//		
//		FCollisionQueryParams Lp = FCollisionQueryParams::DefaultQueryParam;
//		Lp.AddIgnoredComponent(CapsuleCollider);
//		Lp.AddIgnoredActor(GetOwner());
//	
//		FHitResult hit;
//	
//		if (GetWorld()->LineTraceSingleByChannel(hit, OriginL, LRayEndpoint, ECollisionChannel::ECC_WorldStatic, Lp))
//		{
//			//hit.Location;
//			//DebugHelpers::DebugDrawSphere(GetWorld(), hit.Location, 10.0f,  32, FColor::Cyan)
//			DrawDebugSphere(GetWorld(), hit.Location, 10.0f, 8, FColor::Cyan);
//			AnimInfo.LFootIK_Target = hit.Location;
//		}
//	
//		if (GetWorld()->LineTraceSingleByChannel(hit, OriginR, RRayEndpoint, ECollisionChannel::ECC_WorldStatic, Lp))
//		{
//			//hit.Location;
//			//DebugHelpers::DebugDrawSphere(GetWorld(), hit.Location, 10.0f,  32, FColor::Cyan)
//			DrawDebugSphere(GetWorld(), hit.Location, 10.0f, 8, FColor::Yellow);
//			AnimInfo.RFootIK_Target = hit.Location;
//		}
//	
//		// AnimInfo.PelvisDeltaHeight = abs(AnimInfo.RFootIK_Target.Z - AnimInfo.LFootIK_Target.Z);
//	}
