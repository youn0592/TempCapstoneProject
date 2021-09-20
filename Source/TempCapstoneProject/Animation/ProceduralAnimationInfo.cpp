// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralAnimationInfo.h"

FProceduralAnimationInfo::FProceduralAnimationInfo()
	:
	PoseBlendAlpha(0.f),
	Lean(FRotator(0.f)),
	RFootIK_Target(FVector(0.f)),
	LFootIK_Target(FVector(0.f))
{
}
