// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TempCapstoneProject.h"
#include "TempCapstoneProjectGameState.generated.h"

UCLASS()
class TEMPCAPSTONEPROJECT_API ATempCapstoneProjectGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	void BeginPlay() override;

	class USplitScreenGameViewportClient* GameViewportClient;
	void SetSplitscreenBias(float targetBiasPercent, float transitionDuration, bool symmetrical, EScreenDividerMovementStyle style);

	UFUNCTION(NetMulticast, Reliable)
	void NMC_SetSplitscreenBias(float targetBiasPercent, float transitionDuration, bool symmetrical, EScreenDividerMovementStyle style);

	float ClientTargetSplitscreenBias = 0.5f;

	void SetTargetSplitscreenBias(float bias);
	float GetTargetSplitscreenBias();
};
