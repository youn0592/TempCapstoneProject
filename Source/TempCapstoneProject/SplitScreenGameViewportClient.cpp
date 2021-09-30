// Fill out your copyright notice in the Description page of Project Settings.

#include "SplitScreenGameViewportClient.h"

// TODO: FIX CLIENT SPLIT
void USplitScreenGameViewportClient::SetSplitscreenBias(float targetBiasPercent, float transitionDuration, bool symmetrical, EScreenDividerMovementStyle style)
{
	ShiftStyle = style;
	Symmetric = symmetrical;
	TransitionDuration = transitionDuration;

	TargetSplitscreenBias = FMath::Clamp(targetBiasPercent, 0.f, 1.f);

	InitialSplitscreenBias = SplitscreenBias;
	LinearPercentage = 0.0f;

	// instantly set the bias if duration = 0
	if (TransitionDuration == 0)
	{
		SplitscreenBias = TargetSplitscreenBias;
	}
}

void USplitScreenGameViewportClient::Tick(float DeltaTime)
{
	if (SplitscreenBias == TargetSplitscreenBias)
		return;

	LinearPercentage += DeltaTime / TransitionDuration;
	
	if (LinearPercentage >= 1.0)
	{
		LinearPercentage = 0.0f;
		SplitscreenBias = TargetSplitscreenBias;
		return;
	} 

	switch (ShiftStyle)
	{
	case EScreenDividerMovementStyle::Linear:
		SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, LinearPercentage);
		break;

	case EScreenDividerMovementStyle::Smooth:
		SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, 0.5f - 0.5f * cosf(PI * LinearPercentage));
		break;

	case EScreenDividerMovementStyle::MoreSmooth:
		SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, LinearPercentage - (sinf(2 * PI * LinearPercentage)) / (2 * PI));
		break;

	case EScreenDividerMovementStyle::SmoothStart:
		SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, 1 - cosf(0.5f * PI * LinearPercentage));
		break;

	case EScreenDividerMovementStyle::SmoothEnd:
		SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, sinf(0.5f * PI * LinearPercentage));
		break;

	case EScreenDividerMovementStyle::Overshoot:
		{
		float a = LinearPercentage * 1.05448f;
		
		//SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, 1.066946f - 1 / (14 * (LinearPercentage + 0.066946f)) + 0.5f * sinf(5.0f * PI * LinearPercentage * LinearPercentage) * (LinearPercentage - 1) * (LinearPercentage - 1));
		//SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, 1 - 0.003f * (1 - LinearPercentage) * (sinf(10 * PI * a)) / (a * a));
		//SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, tanhf(10 * LinearPercentage) + 0.1f * sin(10 * PI * LinearPercentage * LinearPercentage) * (1 - LinearPercentage));
		//SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, tanhf(6 * LinearPercentage) + 0.2f * sin(6 * LinearPercentage) * ((1 / (4 * LinearPercentage + 4 * 0.2071f)) + 1.2071f));
		//SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, 0.5f * (tanh(10 * a - 0.55) + 1 - 0.5f * cos(5 * PI * a) * (1 - LinearPercentage) * (1 - LinearPercentage)));
		SplitscreenBias = FMath::Lerp(InitialSplitscreenBias, TargetSplitscreenBias, 0.5f * (tanh(10 * a) + 1 - cos(3 * PI * a) * (1 - LinearPercentage) * (1 - LinearPercentage)));
	}
		break;
	}
}

// TODO: ENSURE SAME CHARACTER ON SAME SIDE ON CLIENT AND SERVER
void USplitScreenGameViewportClient::LayoutPlayers()
{
	UpdateActiveSplitscreenType();
	const ESplitScreenType::Type SplitType = GetCurrentSplitscreenConfiguration();

	// Initialize the players
	const TArray<ULocalPlayer*>& PlayerList = GetOuterUEngine()->GetGamePlayers(this);
	
	bool IsServer = GetWorld()->IsServer();

	SplitscreenBias = FMath::Clamp(SplitscreenBias, 0.f, 1.f);
	
	float finalBias = (IsServer) ? SplitscreenBias : 1 - SplitscreenBias;
	
	for (int32 PlayerIdx = 0; PlayerIdx < PlayerList.Num(); PlayerIdx++)
	{

		if (PlayerIdx > 1)
		{
			break;
		}

		if (SplitType < SplitscreenInfo.Num() && PlayerIdx < SplitscreenInfo[SplitType].PlayerData.Num())
		{
			PlayerList[PlayerIdx]->Size.X = PlayerIdx ? 1 - finalBias : finalBias;
			PlayerList[PlayerIdx]->Size.Y = 1;
			PlayerList[PlayerIdx]->Origin.X = PlayerIdx ? finalBias : 0;
			PlayerList[PlayerIdx]->Origin.Y = 0;
			PlayerList[PlayerIdx]->AspectRatioAxisConstraint = EAspectRatioAxisConstraint::AspectRatio_MaintainYFOV;
		}
		else
		{
			PlayerList[PlayerIdx]->Size.X = 0.f;
			PlayerList[PlayerIdx]->Size.Y = 0.f;
			PlayerList[PlayerIdx]->Origin.X = 0.f;
			PlayerList[PlayerIdx]->Origin.Y = 0.f;
		}
	}
}
