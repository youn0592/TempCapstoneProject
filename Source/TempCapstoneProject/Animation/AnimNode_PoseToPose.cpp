// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNode_PoseToPose.h"
/// #include "CapCam.h"
/// #include "Animation/AnimInstanceProxy.h"


void FAnimNode_PoseToPose::Initialize_AnyThread(const FAnimationInitializeContext & Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	FAnimNode_Base::Initialize_AnyThread(Context);

	BlendPoses.Empty();

	InPose4.Initialize(Context);
	InPose3.Initialize(Context);
	InPose2.Initialize(Context);
	InPose1.Initialize(Context);

	CurrentAnimPoseIndex = 0;
	bInputPosesHaveChanged = true;
}

void FAnimNode_PoseToPose::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
//	///A.CacheBones(Context);
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)
//	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)
//	for (int32 ChildIndex = 0; ChildIndex < BlendPoses.Num(); ChildIndex++)
//	{
//		BlendPoses[ChildIndex].CacheBones(Context);
//	}
	InPose4.CacheBones(Context);
	InPose3.CacheBones(Context);
	InPose2.CacheBones(Context);
	InPose1.CacheBones(Context);
}

void FAnimNode_PoseToPose::Update_AnyThread(const FAnimationUpdateContext & Context)
{
	/// Alpha += Context.GetDeltaTime() * Speed;
	/// if (Alpha >= 1.0)
	/// {
	/// 	Alpha = 0;
	/// }
	///	GetEvaluateGraphExposedInputs().Execute(Context);
	///	A.Update(Context);

	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)

	GetEvaluateGraphExposedInputs().Execute(Context);

	InPose4.Update(Context);
	InPose3.Update(Context);
	InPose2.Update(Context);
	InPose1.Update(Context);

	/// QUICK_SCOPE_CYCLE_COUNTER(STAT_FAnimationNode_TwoWayBlend_Update);
//	GetEvaluateGraphExposedInputs().Execute(Context);

	/////InternalBlendAlpha = FMath::Clamp<float>(InternalBlendAlpha, 0.f, 1.f);

//	const bool bNewAIsRelevant = !FAnimWeight::IsFullWeight(Alpha);
//	const bool bNewBIsRelevant = FAnimWeight::IsRelevant(Alpha);

	///// when this flag is true, we'll reinitialize the children
//	if (bResetChildOnActivation)
//	{
//		//if (bNewAIsRelevant && !bAIsRelevant)
//		//{
//		//	FAnimationInitializeContext ReinitializeContext(Context.AnimInstanceProxy);
//		//
//		//	// reinitialize
//		//	A.Initialize(ReinitializeContext);
//		//}
//	
//		FAnimationInitializeContext ReinitializeContext(Context.AnimInstanceProxy);
//
//		for (int32 ChildIndex = 0; ChildIndex < BlendPoses.Num(); ChildIndex++)
//		{
//			BlendPoses[ChildIndex].Initialize(ReinitializeContext);
//		}
//	}
	
//	bAIsRelevant = bNewAIsRelevant;
//	bBIsRelevant = bNewBIsRelevant;
	
	//if (bBIsRelevant)
	//{
	//if (bAIsRelevant)
	//{
	//	// Blend A and B together
	//	A.Update(Context.FractionalWeight(1.0f - Alpha));
	//	B.Update(Context.FractionalWeight(Alpha));
	//}
	//else
	//{
	//	// Take all of B
	//	B.Update(Context);
	//}
//	for (int32 ChildIndex = 0; ChildIndex < BlendPose.Num(); ChildIndex++)
//	{
//		//if(BlendPose[ChildIndex] is relevant?)
//		BlendPose[ChildIndex].Update(Context);
//	}

	//}
	//else
	//{
	//	// Take all of A
	//	A.Update(Context);
	//}
	
	/// TRACE_ANIM_NODE_VALUE(Context, TEXT("Alpha"), InternalBlendAlpha);
}

FQuat FAnimNode_PoseToPose::De_Casteljau(FQuat q0, FQuat q1, FQuat q2, FQuat q3, float t) {

	FQuat slerp_0_1 = FQuat::Slerp(q0, q1, t);
	FQuat slerp_1_2 = FQuat::Slerp(q1, q2, t);
	FQuat slerp_2_3 = FQuat::Slerp(q2, q3, t);

	return FQuat::Slerp( FQuat::Slerp(slerp_0_1, slerp_1_2, t), FQuat::Slerp(slerp_1_2, slerp_2_3, t), t );
}

FQuat FAnimNode_PoseToPose::SlerpNoInvert(FQuat fro, FQuat to, float factor)
{
	float dot = to.X * fro.X + to.Y * fro.Y + to.Z * fro.Z + to.W * fro.W;

	if (FMath::Abs(dot) > 0.9999f)
	{
		return fro;
	}

	float theta = FMath::Acos(dot);
	float sinT = 1.0f / FMath::Sin(theta);

	float newFactor = FMath::Sin(factor * theta) * sinT;

	float invFactor = FMath::Sin((1.0f - factor) * theta) * sinT;

	return FQuat(invFactor * fro.X + newFactor * to.X, invFactor * fro.Y + newFactor * to.Y, invFactor * fro.Z + newFactor * to.Z, invFactor * fro.W + newFactor * to.W);
}

FQuat FAnimNode_PoseToPose::SQUAD(FQuat q0, FQuat q1, FQuat q2, FQuat q3, float t) {

	FQuat outer = SlerpNoInvert(q0, q3, t);
	FQuat inner = SlerpNoInvert(q1, q2, t);

	return SlerpNoInvert(outer, inner, 2*t*(1-t));
}

FQuat FAnimNode_PoseToPose::Intermediate(FQuat q0, FQuat q1, FQuat q2)
{
	FQuat q1inv = q1.Inverse();

	FQuat c1 = q1inv * q2;

	FQuat c2 = q1inv * q0;

	FQuat c3 = c2 + c1; // c2 + c1;
	c3 = c3 * -0.25f;// c3.Scale(-0.25f);
	c3 = c3.Exp();

	FQuat r = q1 * c3;
	return r.GetNormalized();
}

FQuat FAnimNode_PoseToPose::SQUADSegment(FQuat q0, FQuat q1, FQuat q2, FQuat q3, float t)
{
	if (q0.X * q1.X + q0.Y * q1.Y + q0.Z * q1.Z + q0.W * q1.W < 0)
	{
		q1 *= -1;
	}
	if (q1.X * q2.X + q1.Y * q2.Y + q1.Z * q2.Z + q1.W * q2.W < 0)
	{
		q2 *= -1;
	}	
	if (q2.X * q3.X + q2.Y * q3.Y + q2.Z * q3.Z + q2.W * q3.W < 0)
	{
		q3 *= -1;
	}

	FQuat qa = Intermediate(q0, q1, q2);
	FQuat qb = Intermediate(q1, q2, q3);
	return SQUAD(q1, qa, qb, q2, t);
}

void FAnimNode_PoseToPose::Evaluate_AnyThread(FPoseContext & Output)
{

	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
	ANIM_MT_SCOPE_CYCLE_COUNTER(BlendPosesInGraph, !IsInGameThread());
	
	const int32 NumPoses = BlendPoses.Num();

	//if (bInputPosesHaveChanged || BlendPoses.Num() == 0)
	//{
		CurrentAnimPoseIndex = 0;

		BlendPoses.Empty();
		FPoseContext TempPose1(Output);
		FPoseContext TempPose2(Output);
		FPoseContext TempPose3(Output);
		FPoseContext TempPose4(Output);

		InPose1.Evaluate(TempPose1);
		BlendPoses.Add((TempPose1.Pose));

		InPose2.Evaluate(TempPose2);
		BlendPoses.Add((TempPose2.Pose));

		InPose3.Evaluate(TempPose3);
		BlendPoses.Add((TempPose3.Pose));

		InPose4.Evaluate(TempPose4);
		BlendPoses.Add((TempPose4.Pose));

		//InPose1.Evaluate(TempPose);
		//AnimPose_Current = TempPose.Pose;

		//bInputPosesHaveChanged = false;
	//}

	if ((NumPoses > 0))
	{

		float PerPoseAlpha = fmod(Alpha * 4.f, 1.f);
		lastAnimPoseIndex = (int)(Alpha * 4.0f) % 4;

		CurrentAnimPoseIndex = (lastAnimPoseIndex + 1) % 4;
		nextAnimPoseIndex	 = (CurrentAnimPoseIndex + 1) % 4;
		futureAnimPoseIndex  = (nextAnimPoseIndex + 1) % 4;

		FAnimationPoseData OutAnimationPoseData(Output);
		FCompactPose& OutPose = OutAnimationPoseData.GetPose();
		for (FCompactPoseBoneIndex BoneIndex : OutPose.ForEachBoneIndex())
		{
			/// OutPose[BoneIndex] = BlendPoses[CurrentAnimPoseIndex][BoneIndex];
			/// OutPose[BoneIndex].SetLocation(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetLocation());
			/// OutPose[BoneIndex].SetLocation((FMath::CubicCRSplineInterpSafe(BlendPoses[lastAnimPoseIndex][BoneIndex].GetLocation(), BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetLocation(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetLocation(), BlendPoses[futureAnimPoseIndex][BoneIndex].GetLocation(), 0.5, 0.5, 0.5, 0.5, PerPoseAlpha)));
			OutPose[BoneIndex].SetLocation(FMath::Lerp(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetLocation(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetLocation(), PerPoseAlpha));
			/// OutPose[BoneIndex].SetLocation(FMath::CubicCRSplineInterp(BlendPoses[lastAnimPoseIndex][BoneIndex].GetLocation(), BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetLocation(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetLocation(), 0.5f, 0.5f, 0.5f, 0.5f, PerPoseAlpha));
			
			// FVector outqAxis = FMath::CubicCRSplineInterpSafe(BlendPoses[lastAnimPoseIndex][BoneIndex].GetRotation().GetRotationAxis(), BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetRotation().GetRotationAxis(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetRotation().GetRotationAxis(), BlendPoses[futurePoseIndex][BoneIndex].GetRotation().GetRotationAxis(), Beta, Beta, Beta, Beta, PerPoseAlpha);
			// FQuat outq = FQuat(outqAxis, FMath::Lerp(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetRotation().GetAngle(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetRotation().GetAngle(), PerPoseAlpha));
			// FQuat outq = FQuat::Slerp(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetRotation(), PerPoseAlpha);

			//OutPose[BoneIndex].SetRotation(outq);

			OutPose[BoneIndex].SetRotation((SQUADSegment(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[futureAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[lastAnimPoseIndex][BoneIndex].GetRotation(), PerPoseAlpha)));
			// OutPose[BoneIndex].SetRotation((FQuat::Squad(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[lastAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[futureAnimPoseIndex][BoneIndex].GetRotation(), PerPoseAlpha)));

			// OutPose[BoneIndex].SetRotation((De_Casteljau(BlendPoses[lastAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[futureAnimPoseIndex][BoneIndex].GetRotation(), PerPoseAlpha)));
			// OutPose[BoneIndex].SetRotation(FMath::CubicInterp(AnimationPoseOneData.GetPose()[BoneIndex].GetRotation(), (rA - rB).GetNormalized().Quaternion(), AnimationPoseTwoData.GetPose()[BoneIndex].GetRotation(), (rB - rA).GetNormalized().Quaternion(), Alpha));
			// OutPose[BoneIndex].SetRotation(FMath::Lerp(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetRotation(), Beta == 2 ? PerPoseAlpha : Beta));
			// [BoneIndex].SetRotation(FQuat::Slerp(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetRotation(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetRotation(), PerPoseAlpha));

			OutPose[BoneIndex].SetScale3D(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetScale3D());
			///OutPose[BoneIndex].SetScale3D(FMath::Lerp(BlendPoses[CurrentAnimPoseIndex][BoneIndex].GetScale3D(), BlendPoses[nextAnimPoseIndex][BoneIndex].GetScale3D(), PerPoseAlpha));
			
			// OutPose[BoneIndex].Blend(AnimationPoseOneData.GetPose()[BoneIndex], AnimationPoseTwoData.GetPose()[BoneIndex], Alpha);

			// BlendTransform<ETransformBlendMode::Overwrite>(BlendPoses[CurrentAnimPoseIndex][BoneIndex], OutPose[BoneIndex], PerPoseAlpha);
			// BlendTransform<ETransformBlendMode::Accumulate>(BlendPoses[nextAnimPoseIndex][BoneIndex], OutPose[BoneIndex], 1 - PerPoseAlpha);
		}

		OutPose.NormalizeRotations();
	}
	else {
		Output.ResetToRefPose();
	}


	// Evaluate the input
	//A.Evaluate_AnyThread(Output);
	///
	//if (bBIsRelevant)
	//{
	//	if (bAIsRelevant)
	//	{
//			FPoseContext Pose1(Output);
//			FPoseContext Pose2(Output);
///	
///			A.Evaluate(Pose1);
///			B.Evaluate(Pose2);
///	
//			FAnimationPoseData BlendedAnimationPoseData(Output);
//			const FAnimationPoseData AnimationPoseOneData(Pose1);
//			const FAnimationPoseData AnimationPoseTwoData(Pose2);
///
///			//-----------------------------------------------------------------------------
///			
///			FCompactPose& OutPose = BlendedAnimationPoseData.GetPose();
///			FBlendedCurve& OutCurve = BlendedAnimationPoseData.GetCurve();
///			FStackCustomAttributes& OutAttributes = BlendedAnimationPoseData.GetAttributes();
///
///			const float WeightOfPoseTwo = 1.f - Alpha;
///
///			for (FCompactPoseBoneIndex BoneIndex : AnimationPoseOneData.GetPose().ForEachBoneIndex())
///			{
///				OutPose[BoneIndex].SetLocation(FMath::CubicInterp(AnimationPoseOneData.GetPose()[BoneIndex].GetLocation(), FVector(0.0f), AnimationPoseTwoData.GetPose()[BoneIndex].GetLocation(), FVector(1.0f), Alpha));
///				
///				FRotator rA = AnimationPoseOneData.GetPose()[BoneIndex].GetRotation().Rotator();
///				FRotator rB = AnimationPoseTwoData.GetPose()[BoneIndex].GetRotation().Rotator();
///				
///				OutPose[BoneIndex].SetRotation(FMath::Lerp(AnimationPoseOneData.GetPose()[BoneIndex].GetRotation(), AnimationPoseTwoData.GetPose()[BoneIndex].GetRotation(), Alpha));
///				//OutPose[BoneIndex].SetRotation(FMath::CubicInterp(AnimationPoseOneData.GetPose()[BoneIndex].GetRotation(), (rA - rB).GetNormalized().Quaternion(), AnimationPoseTwoData.GetPose()[BoneIndex].GetRotation(), (rB - rA).GetNormalized().Quaternion(), Alpha));
///				OutPose[BoneIndex].SetScale3D(FMath::Lerp(AnimationPoseOneData.GetPose()[BoneIndex].GetScale3D(), AnimationPoseTwoData.GetPose()[BoneIndex].GetScale3D(), Alpha));
///				
///				///OutPose[BoneIndex].Blend(AnimationPoseOneData.GetPose()[BoneIndex], AnimationPoseTwoData.GetPose()[BoneIndex], Alpha);
///
///				//BlendTransform<ETransformBlendMode::Overwrite>(AnimationPoseOneData.GetPose()[BoneIndex], OutPose[BoneIndex], Alpha);
///				//BlendTransform<ETransformBlendMode::Accumulate>(AnimationPoseTwoData.GetPose()[BoneIndex], OutPose[BoneIndex], WeightOfPoseTwo);
///			}
///
///			// Ensure that all of the resulting rotations are normalized
///			OutPose.NormalizeRotations();
///
///			OutCurve.Lerp(AnimationPoseOneData.GetCurve(), AnimationPoseTwoData.GetCurve(), WeightOfPoseTwo);
///			FCustomAttributesRuntime::BlendAttributes({ AnimationPoseOneData.GetAttributes(), AnimationPoseTwoData.GetAttributes() }, { Alpha, WeightOfPoseTwo }, { 0, 1 }, OutAttributes);
			
			
	//-----------------------------------------------------------------------------

	/// FAnimationRuntime::BlendTwoPosesTogether(AnimationPoseOneData, AnimationPoseTwoData, Alpha, BlendedAnimationPoseData);
	//	}
	//	else
	//	{
	//		B.Evaluate(Output);
	//	}
	//}
	//else
	//{
	//	A.Evaluate(Output);
	//}
}

void FAnimNode_PoseToPose::GatherDebugData(FNodeDebugData & DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	
	const int32 ChildIndex = CurrentAnimPoseIndex;
	const float AlphaVal = Alpha;

	FString DebugLine = DebugData.GetNodeName(this);
	DebugData.AddDebugItem(DebugLine);

	DebugLine += FString::Printf(TEXT("Active: %d, Alpha: %.1f%%)"), ChildIndex, Alpha);
	DebugData.AddDebugItem(DebugLine);

	// A.GatherDebugData(DebugData);
	///DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	///A.GatherDebugData(DebugData.BranchFlow(1.f - InternalBlendAlpha));
	///B.GatherDebugData(DebugData.BranchFlow(InternalBlendAlpha));
}

