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

	TSubclassOf <ACharacter> pRogueBP;
	TSubclassOf <ACharacter> pPaladinBP;
	TSubclassOf<APawn> pDummyBP;

	uint8 PlayerCount;

	// Game Startup methods
	void DelaySpawnUntilLevelLoaded();
	void SpawnPawnAndPosess(APlayerController* NewPlayer);

	virtual void PostLogin(APlayerController* NewPlayer) override;

	FTimerHandle SpawnDelayHandle;

public:

	//	void SetDefaultPawnToDummy();
	
	bool bLevelHasLoaded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		bool PlayerOneIsPaladin = true;

	ATempCapstoneProjectGameMode();

	// method used to modify the splitscreen split bias
	UFUNCTION(BlueprintCallable)
		void SetSplitscreenBias(float TargetBias, float TransitionDuration = 1.0f, EScreenDividerMovementStyle TransitionStyle = EScreenDividerMovementStyle::Linear);

	TArray<class APlayerController*> PlayerControllerList;

	virtual void BeginPlay() override;

};



