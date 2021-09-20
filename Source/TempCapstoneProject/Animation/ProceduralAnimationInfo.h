// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralAnimationInfo.generated.h"

USTRUCT(BlueprintType)
struct TEMPCAPSTONEPROJECT_API FProceduralAnimationInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float PoseBlendAlpha = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		FRotator Lean = FRotator(0);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		FTransform RFootIK_Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		FTransform LFootIK_Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float RFootIK_BlendFactor = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Info")
		float LFootIK_BlendFactor = 1.0f;

	//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	//		float LFootIK_Blend = 0.0f;
	//	
	//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	//		float RFootIK_Blend = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		float PelvisDeltaHeight = 0.0f;

	FProceduralAnimationInfo();
};