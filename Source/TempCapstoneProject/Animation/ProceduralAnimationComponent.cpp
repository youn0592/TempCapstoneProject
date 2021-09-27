// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralAnimationComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Net/UnrealNetwork.h"

UProceduralAnimationComponent::UProceduralAnimationComponent() : AnimInfo()
{
	PrimaryComponentTick.bCanEverTick = true;
	// WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelMesh"));
	SetIsReplicatedByDefault(true);
	//IK_SquareDistanceSinceReplant = 0;
	//BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
}

void UProceduralAnimationComponent::Setup()
{
	ACharacter* C = Cast<ACharacter>(GetOwner());
	
	CapsuleCollider = C->GetCapsuleComponent();
	MovementComp = C->GetCharacterMovement();

	// For debugging animation
	// WheelMesh->SetRelativeScale3D(FVector(MinStrideLength, 1, MinStrideLength));
	// WheelMesh->SetRelativeLocationAndRotation(FVector(0, 0, MinStrideLength * 100 * 0.5f - CapsuleCollider->GetScaledCapsuleHalfHeight()), CapsuleCollider->GetRelativeRotation());
	// WheelMesh->AttachToComponent(CapsuleCollider, FAttachmentTransformRules::KeepRelativeTransform);

	FVector CapsuleBase = CapsuleCollider->GetRelativeLocation() - FVector(0, 0, CapsuleCollider->GetScaledCapsuleHalfHeight());
	FVector LegOffset_Right = CapsuleCollider->GetRightVector() * StepWidth;

	IK_LastL.SetLocation(CapsuleBase - LegOffset_Right);
	IK_LastL.SetRotation(CapsuleCollider->GetRelativeRotation().Quaternion());
	IK_LastR.SetLocation(CapsuleBase + LegOffset_Right);
	IK_LastR.SetRotation(CapsuleCollider->GetRelativeRotation().Quaternion());

	IK_NextL = IK_LastL;
	IK_NextL = IK_LastR;

	IK_PredictedCapsuleLocation = CapsuleBase;

	//IK_LFootSocket = MovementComp->GetCharacterOwner()->GetMesh()->GetSocketByName("IK_LFootSocket");
	//IK_RFootSocket = MovementComp->GetCharacterOwner()->GetMesh()->GetSocketByName("IK_RFootSocket");
	CharacterMesh = C->GetMesh();
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


	if (CapsuleCollider == nullptr || MovementComp == nullptr)
		return;
	
	CalcPelvisZ = 0;
	float ClampedDelta = FMath::Clamp(DeltaTime, 0.f, 0.05f);

	FVector Worldvel = CapsuleCollider->GetComponentVelocity();
	HandleHamsterWheel(ClampedDelta, Worldvel);
	HandleLean(ClampedDelta, Worldvel);
	HandleIK(ClampedDelta);

	if(FMath::Abs(previousCalcZ - CalcPelvisZ) < 30)
	AnimInfo.PelvisDeltaHeight = FMath::FInterpTo(AnimInfo.PelvisDeltaHeight, CalcPelvisZ, ClampedDelta, 20.f - AnimInfo.WalkRunBlend*15);

	previousCalcZ = CalcPelvisZ;
}

void UProceduralAnimationComponent::HandleHamsterWheel(float DeltaTime, FVector worldVelocity)
{
	FVector WorldVel2D = worldVelocity;
	WorldVel2D.Z = 0;

	AnimInfo.WalkRunBlend = FMath::Clamp(WorldVel2D.Size2D() / MovementComp->GetMaxSpeed(), 0.f, 1.f);
	float StrideLength = FMath::Lerp(MinStrideLength, MaxStrideLength, AnimInfo.WalkRunBlend);

	//WheelMesh->SetRelativeLocation(FVector(0, 0, StrideLength * 100 * 0.5f - CapsuleCollider->GetScaledCapsuleHalfHeight()));
	//WheelMesh->SetRelativeScale3D(FVector(StrideLength, 1, StrideLength));

	float WheelRotationThisFrame = WorldVel2D.Size() * 0.01f * DeltaTime * 180.f / (StrideLength * 0.5f * PI);
	float PercentAroundWheelThisFrame = WorldVel2D.Size() * 0.01f * DeltaTime / (2 * PI * StrideLength * 0.5f);

	AnimInfo.PoseBlendAlpha += PercentAroundWheelThisFrame;

	if (AnimInfo.WalkRunBlend < 0.005f)
	{
		AnimInfo.PoseBlendAlpha = fmod(AnimInfo.PoseBlendAlpha, 1.0f);

		if(AnimInfo.PoseBlendAlpha <= 0.5f)
		AnimInfo.PoseBlendAlpha = FMath::Lerp(AnimInfo.PoseBlendAlpha, 0.f, DeltaTime);
		else
		AnimInfo.PoseBlendAlpha = FMath::Lerp(AnimInfo.PoseBlendAlpha, 1.f, DeltaTime);
	}

	//	IK_TriggerFactor += 2 * PercentAroundWheelThisFrame;
	//	
	//	if (IK_TriggerFactor > 0.5f)
	//	{
	//		IK_MustReplant = true;
	//		IK_TriggerFactor = 0.f;
	//	}

	//	AnimInfo.RFootIK_BlendFactor = FMath::Clamp(sinf(2 * PI * AnimInfo.PoseBlendAlpha), 0.f, 1.f);
	//	AnimInfo.LFootIK_BlendFactor = FMath::Clamp(sinf(PI * (2 * AnimInfo.PoseBlendAlpha + 0.5f)), 0.f, 1.f);

	// AnimInfo.PoseBlendAlpha = fmod(AnimInfo.PoseBlendAlpha, 1.0f);
	//WheelMesh->AddLocalRotation(FRotator(-WheelRotationThisFrame, 0, 0));
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

	//	CalcPelvisZ = FMath::Clamp(FMath::Lerp(CalcPelvisZ, -50*LeanOvershoot.Z, FMath::Clamp(40 * DeltaTime, 0.f, 1.f) ), -20.f,20.f);
	//	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("%f"), CalcPelvisZ));

	//--Play with this to scale leaning caused by jumping.
	float FinJumpLeanScale = AccelVector.Z < 0 ? 1.f : 0.2f;

	//--Calculate final Lean amount by lerping to the sum of velocity-based, acceleration-based and Jump-based leaning.
	//--Clamped velocity-based side-leaning and jump based leaning. (RECONSIDER CLAMPING OR EXPOSE SOME OF THOSE VALUES) (TEST FOR FRAME DEPENDANCY ISSUES)
	AnimInfo.Lean = FMath::Lerp
	(
			AnimInfo.Lean
		,
			LeanScale * FRotator
			(
				FMath::Clamp(FVector::DotProduct(worldVelocity, CapsuleCollider->GetRightVector()), -LeanLimit, LeanLimit),
				0,
				FVector::DotProduct(worldVelocity, CapsuleCollider->GetForwardVector())
			)
			+ 
			FRotator
			(
			FMath::Clamp(LeanOvershootScale * LeanOvershoot.Y, -30.f, 30.f),
			0,
			FMath::Clamp( LeanOvershootScale * LeanOvershoot.X + JumpLeanScale * FinJumpLeanScale * FMath::Clamp(LeanOvershoot.Z, -10.f, 10.f) , -30.f , 30.f)
			)
		,
			FMath::Clamp(10.0f * DeltaTime, 0.f, 1.f)
	);
}

void UProceduralAnimationComponent::HandleIK(float DeltaTime)
{
	float RDistance = 900.0f;
	float LDistance = 900.0f;

	float RFinalAlpha = 0.0f;
	float LFinalAlpha = 0.0f;

	float MinDistanceFromRoot = 48.f;

	bool lhit = false;
	bool rhit = false;

	FHitResult hit;
	
	FVector lfootRayOrigin = CharacterMesh->GetSocketTransform(FName("IK_LFootSocket")).GetLocation();
	FVector rfootRayOrigin = CharacterMesh->GetSocketTransform(FName("IK_RFootSocket")).GetLocation();
	
	FVector lNormal = FVector::UpVector;
	FVector rNormal = FVector::UpVector;

	float RayRange = CapsuleCollider->GetScaledCapsuleHalfHeight() + 55;
	
	lfootRayOrigin.Z = rfootRayOrigin.Z = CapsuleCollider->GetRelativeLocation().Z;
	
	FCollisionQueryParams qp;
	qp.AddIgnoredActor(GetOwner());
	
	if (GetWorld()->LineTraceSingleByChannel(hit, lfootRayOrigin, lfootRayOrigin + FVector::DownVector * RayRange /*lfoot.GetRotation().GetForwardVector()*/, ECollisionChannel::ECC_WorldStatic, qp))
	{
		// DrawDebugSphere(GetWorld(), hit.Location, 4.0f, 8, FColor::Red);
		LDistance = hit.Distance;
		lNormal = hit.Normal;
		lhit = true;
	}
	//	else
	//	{
	//		if (GetWorld()->LineTraceSingleByChannel(hit, -CapsuleCollider->GetRightVector() * 10 + lfootRayOrigin, lfootRayOrigin + FVector::DownVector * RayRange /*lfoot.GetRotation().GetForwardVector()*/, ECollisionChannel::ECC_WorldStatic, qp))
	//		{
	//			DrawDebugSphere(GetWorld(), hit.Location, 5.0f, 8, FColor::Green);
	//			LDistance = hit.Distance - 5;
	//			lNormal = hit.Normal;
	//			lhit = true;
	//		}
	//	}
	
	if (GetWorld()->LineTraceSingleByChannel(hit, rfootRayOrigin, rfootRayOrigin + FVector::DownVector * RayRange /*lfoot.GetRotation().GetForwardVector()*/, ECollisionChannel::ECC_WorldStatic, qp))
	{
		// DrawDebugSphere(GetWorld(), hit.Location, 4.0f, 8, FColor::Red);
		RDistance = hit.Distance;
		rNormal = hit.Normal;
		rhit = true;
	}
	//	else
	//	{
	//		
	//		if (GetWorld()->LineTraceSingleByChannel(hit, CapsuleCollider->GetRightVector() * 10 + rfootRayOrigin,rfootRayOrigin + FVector::DownVector * RayRange /*lfoot.GetRotation().GetForwardVector()*/, ECollisionChannel::ECC_WorldStatic, qp))
	//		{
	//			DrawDebugSphere(GetWorld(), hit.Location, 5.0f, 8, FColor::Green);
	//			RDistance = hit.Distance - 5;
	//			rNormal = hit.Normal;
	//			rhit = true;
	//		}
	//	}


	if (lhit || rhit)
	{

		FVector rootOrigin = CapsuleCollider->GetRelativeLocation();

		FVector newOriginL = FVector(rfootRayOrigin.X, rfootRayOrigin.Y, rootOrigin.Z);
		FVector newOriginR = FVector(lfootRayOrigin.X, lfootRayOrigin.Y, rootOrigin.Z);

		float Dt = rootOrigin.Z - (CapsuleCollider->GetRelativeLocation().Z - CapsuleCollider->GetScaledCapsuleHalfHeight());

		//	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("%f"), (Dt - MinDistanceFromRoot)));
		//	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%f"), RDistance));
		//	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("%f"), LDistance));
		//if (FMath::IsNearlyEqual(RDistance, LDistance, 1) && RDistance - CapsuleCollider->GetScaledCapsuleHalfHeight() > 1)
		//	RDistance = LDistance = 0;

		RFinalAlpha = RDistance / (Dt - MinDistanceFromRoot) - 2.f ;
		LFinalAlpha = LDistance / (Dt - MinDistanceFromRoot) - 2.f ;

		//	AnimInfo.RFootIK_Rotator = FRotator(FMath::Atan2(rNormal.Y, rNormal.Z), 0, FMath::Atan2(rNormal.Y,rNormal.Z));
		//	AnimInfo.LFootIK_Rotator = FRotator(FMath::Atan2(lNormal.Y, lNormal.Z), 0, FMath::Atan2(lNormal.Y,lNormal.Z));

		float rRoll = 0; float rPitch = 0; float lRoll = 0; float lPitch = 0;

		UKismetMathLibrary::GetSlopeDegreeAngles(CapsuleCollider->GetRightVector(), rNormal, FVector::UpVector, rRoll, rPitch);
		UKismetMathLibrary::GetSlopeDegreeAngles(CapsuleCollider->GetRightVector(), lNormal, FVector::UpVector, lRoll, lPitch);

		AnimInfo.RFootIK_Rotator = FRotator(-rPitch, 0, -rRoll);
		AnimInfo.LFootIK_Rotator = FRotator(-lPitch, 0, -lRoll);

		// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("%f,%f"), AnimInfo.RLFootIK_BlendFactor.Y, AnimInfo.RLFootIK_BlendFactor.X));

		CalcPelvisZ += -abs(RDistance  - LDistance);
		if (abs(CalcPelvisZ) > 50)
		{
			CalcPelvisZ = 0;
			RFinalAlpha = LFinalAlpha = 0;
		}
	}
	else {
		CalcPelvisZ = 0;
	}

	AnimInfo.RLFootIK_BlendFactor.X = RFinalAlpha;
	AnimInfo.RLFootIK_BlendFactor.Y = LFinalAlpha;
}

void UProceduralAnimationComponent::UpdateIKTarget()
{
	//	FVector targetPos = IK_PredictedCapsuleLocation;
	//	
	//	switch (IK_ReachingFoot)
	//	{
	//		case FootEnum::LeftFoot:
	//			targetPos -= CapsuleCollider->GetRightVector() * StepWidth;
	//			IK_LastL = IK_NextL;
	//			IK_NextL.SetLocation(targetPos);
	//			AnimInfo.LFootIK_Target = IK_NextL;
	//			IK_ReachingFoot = FootEnum::RightFoot;
	//			break;
	//	
	//		case FootEnum::RightFoot:
	//			targetPos += CapsuleCollider->GetRightVector() * StepWidth;
	//			IK_LastR = IK_NextR;
	//			IK_NextR.SetLocation(targetPos);
	//			AnimInfo.RFootIK_Target = IK_NextR;
	//			IK_ReachingFoot = FootEnum::LeftFoot;
	//			break;
	//	}





	// FVector CapsuleBase = CapsuleCollider->GetRelativeLocation() - FVector(0, 0, CapsuleCollider->GetScaledCapsuleHalfHeight());
	// FVector LegOffset_Right = CapsuleCollider->GetRightVector() * StepWidth;

	// FVector PredictedCapsuleLocation = CapsuleBase + LegOffset_Right;
	// FVector LOrigin = CapsuleBase - LegOffset_Right;

	// FVector TargetL = LOrigin + CapsuleCollider->GetComponentVelocity() * deltaTime + CapsuleCollider->GetComponentAcceler * deltaTime
}

void UProceduralAnimationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UProceduralAnimationComponent, CapsuleCollider);
	DOREPLIFETIME(UProceduralAnimationComponent, MovementComp);
}

// CODE DUMP:


			// DrawDebugSphere(GetWorld(), lfoot.GetLocation(), 3.0f, 8, FColor::Yellow);
			// DrawDebugSphere(GetWorld(), rfoot.GetLocation(), 3.0f, 8, FColor::Yellow);
			// 
			// DrawDebugSphere(GetWorld(), lfoot.GetLocation() + lfoot.GetRotation().GetForwardVector() * 20.f, 3.0f, 8, FColor::Red);
			// DrawDebugSphere(GetWorld(), rfoot.GetLocation() + rfoot.GetRotation().GetForwardVector() * 20.f, 3.0f, 8, FColor::Red);

			// DrawDebugSphere(GetWorld(), lfoot + FVector::DownVector * 20.f, 3.0f, 8, FColor::Red);
			// DrawDebugSphere(GetWorld(), rfoot + FVector::DownVector * 20.f, 3.0f, 8, FColor::Red);

			// AnimInfo.PelvisDeltaHeight += FMath::Lerp

	// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%f,%f,%f"), worldVelocity.X, worldVelocity.Y, worldVelocity.Z));

	//  FVector CapsuleBase = CapsuleCollider->GetRelativeLocation() - FVector(0, 0, CapsuleCollider->GetScaledCapsuleHalfHeight());
	//	IK_PredictedCapsuleLocation = CapsuleBase + CapsuleCollider->GetComponentVelocity() * IK_PredictionRate + MovementComp->GetCurrentAcceleration() * IK_PredictionRate * IK_PredictionRate;
	//	
	//	// IK_SquareDistanceSinceReplant += CapsuleCollider->GetComponentVelocity().Size() * 0.01f * DeltaTime + DeltaTime * 5;
	//	IK_SquareDistanceSinceReplant += DeltaTime * (5 / FMath::Clamp(CapsuleCollider->GetComponentVelocity().SizeSquared(),0.f,5.f));
	//	
	//	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("%f"), IK_SquareDistanceSinceReplant));
	//	
	//	//if (> IK_ReplantRate)
	//	if(IK_MustReplant || IK_SquareDistanceSinceReplant > IK_ReplantRate)
	//	{
	//		UpdateIKTarget();
	//		IK_MustReplant = false;
	//		GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Cyan, TEXT("REPLANT"));
	//		IK_SquareDistanceSinceReplant = 0;
	//	}
	//	
	//	DrawDebugSphere(GetWorld(), IK_NextL.GetLocation(), 10.0f, 8, FColor::Cyan);
	//	DrawDebugSphere(GetWorld(), IK_NextR.GetLocation(), 10.0f, 8, FColor::Red);
	//	
	//	DrawDebugSphere(GetWorld(), IK_LastL.GetLocation(), 10.0f, 8, FColor::Black);
	//	DrawDebugSphere(GetWorld(), IK_LastR.GetLocation(), 10.0f, 8, FColor::Black);
	//	
	//	DrawDebugSphere(GetWorld(), IK_PredictedCapsuleLocation, 10.0f, 8, FColor::White);
	// 
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
