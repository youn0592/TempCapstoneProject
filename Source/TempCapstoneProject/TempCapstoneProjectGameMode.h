// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

// needed for access to splitscreen enum
#include "TempCapstoneProject.h"

#include "TempCapstoneProjectGameMode.generated.h"

UCLASS(minimalapi)
class ATempCapstoneProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	// pointer to custom game viewport class for splitscreen purposes
	class USplitScreenGameViewportClient* GameViewportClient = nullptr;

public:
	ATempCapstoneProjectGameMode();

	// method used to modify the splitscreen split bias
	UFUNCTION(BlueprintCallable)
		void SetSplitscreenBias(float TargetBias, float TransitionDuration = 1.0f, EScreenDividerMovementStyle TransitionStyle = EScreenDividerMovementStyle::Linear);

	virtual void BeginPlay() override;

};



