// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "TempCapstoneProject.h"
#include "SplitScreenGameViewportClient.generated.h"


UCLASS()
class TEMPCAPSTONEPROJECT_API USplitScreenGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

protected:

	float LinearPercentage = 0.0f;
	float TransitionDuration = 0.0f;
	float SplitscreenBias = 0.5f;
	float InitialSplitscreenBias = 0.5f;
	float TargetSplitscreenBias = 0.5f;

	EScreenDividerMovementStyle ShiftStyle = EScreenDividerMovementStyle::Linear;
	bool Symmetric = false;

public:

	UFUNCTION(BlueprintCallable)
	void SetSplitscreenBias(float targetBiasPercent, float transitionDuration = 1.f, bool symmetrical = false, EScreenDividerMovementStyle style = EScreenDividerMovementStyle::Linear);

	virtual void LayoutPlayers() override;
	virtual void Tick(float DeltaTime) override;
};
