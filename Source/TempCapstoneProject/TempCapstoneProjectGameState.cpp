// Fill out your copyright notice in the Description page of Project Settings.


#include "TempCapstoneProjectGameState.h"
#include "SplitScreenGameViewportClient.h"
#include "Net/UnrealNetwork.h"


void ATempCapstoneProjectGameState::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld())
        GameViewportClient = Cast<USplitScreenGameViewportClient>(GetWorld()->GetGameViewport());
}

// TODO: FIX CLIENT SPLIT
void ATempCapstoneProjectGameState::SetSplitscreenBias(float targetBiasPercent, float transitionDuration, bool symmetrical, EScreenDividerMovementStyle style)
{
    if (GameViewportClient)
    {
        NMC_SetSplitscreenBias( targetBiasPercent,  transitionDuration,  symmetrical,  style);
    }
}

void ATempCapstoneProjectGameState::NMC_SetSplitscreenBias_Implementation(float targetBiasPercent, float transitionDuration, bool symmetrical, EScreenDividerMovementStyle style)
{
    if (GameViewportClient)
    {
        GameViewportClient->SetSplitscreenBias(targetBiasPercent,  transitionDuration,  symmetrical,  style);
    }
}

void ATempCapstoneProjectGameState::SetTargetSplitscreenBias(float bias)
{
    ClientTargetSplitscreenBias = bias;
}

float ATempCapstoneProjectGameState::GetTargetSplitscreenBias()
{
    return ClientTargetSplitscreenBias;
}
