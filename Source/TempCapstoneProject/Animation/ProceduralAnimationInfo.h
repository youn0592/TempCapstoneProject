// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralAnimationInfo.generated.h"

USTRUCT(BlueprintType)
struct TEMPCAPSTONEPROJECT_API FProceduralAnimationInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInfo")
		float PoseBlendAlpha = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimInfo")
		FRotator Lean = FRotator(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInfo")
		FVector2D RLFootIK_BlendFactor = FVector2D(0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInfo")
		FRotator RFootIK_Rotator = FRotator(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInfo")
		FRotator LFootIK_Rotator = FRotator(0);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimInfo")
		float WalkRunBlend = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimInfo")
		float PelvisDeltaHeight = 0.0f;

	FProceduralAnimationInfo();
};