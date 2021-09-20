// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Animation/AnimNodeBase.h"
#include "Animation/InputScaleBias.h"
#include "ProceduralAnimationComponent.h"
#include "AnimNode_PoseToPose.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct TEMPCAPSTONEPROJECT_API FAnimNode_PoseToPose : public FAnimNode_Base
{
	GENERATED_BODY()
		
protected:

	int lastAnimPoseIndex = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pins, meta = (PinShownByDefault))
	int CurrentAnimPoseIndex = 0;
	int	nextAnimPoseIndex = 1;
	int	futureAnimPoseIndex = 2;

	UPROPERTY(EditAnywhere, Category = Option)
		uint8 bResetChildOnActivation : 1;

public:

	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite, Category = Pins, meta = (PinShownByDefault))
		FPoseLink InPose1;
	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite, Category = Pins, meta = (PinShownByDefault))
		FPoseLink InPose2;
	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite, Category = Pins, meta = (PinShownByDefault))
		FPoseLink InPose3;
	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite, Category = Pins, meta = (PinShownByDefault))
		FPoseLink InPose4;

	// UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite)
		TArray<FCompactPose> BlendPoses;

	/** The float value that controls the alpha blending when the alpha input type is set to 'Float' */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Runtime, meta = (AlwaysAsPin))
		float Alpha;

public:


//#if WITH_EDITOR
//	virtual void AddPose()
//	{
//		new (BlendPose) FPoseLink();
//	}
//
//	virtual void RemovePose(int32 PoseIndex)
//	{
//		BlendPose.RemoveAt(PoseIndex);
//	}
//#endif

	FAnimNode_PoseToPose()
		: 
		CurrentAnimPoseIndex(0),
		bResetChildOnActivation(false),
		Alpha(0.0f)
	{
	}

	//FAnimNode_PoseToPose();
	
	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext & Output) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// "Cubic" slerp https://splines.readthedocs.io/en/latest/rotation/de-casteljau.html
	FQuat De_Casteljau(FQuat q0, FQuat q1, FQuat q2, FQuat q3, float t);

	// "SQUAD" interpolation https://www.geometrictools.com/Documentation/Quaternions.pdf
	FQuat SQUAD(FQuat q0, FQuat q1, FQuat q2, FQuat q3, float t);
	FQuat SlerpNoInvert(FQuat fro, FQuat to, float factor);
	FQuat SQUADSegment(FQuat q0, FQuat q1, FQuat q2, FQuat q3, float t);
	FQuat Intermediate(FQuat q0, FQuat q1, FQuat q2);

};
