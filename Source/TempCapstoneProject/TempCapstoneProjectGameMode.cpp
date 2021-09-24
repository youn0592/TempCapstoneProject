// Copyright Epic Games, Inc. All Rights Reserved.

#include "TempCapstoneProjectGameMode.h"
#include "TempCapstoneProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "SplitScreenGameViewportClient.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/LevelStreaming.h"

ATempCapstoneProjectGameMode::ATempCapstoneProjectGameMode()
{
	// Get all our required bps and controllers
	static ConstructorHelpers::FClassFinder<ACharacter> PaladinBP_Getter(TEXT("/Game/Blueprints/Characters/BP_PaladinCharacter"));
	static ConstructorHelpers::FClassFinder<ACharacter> RogueBP_Getter(TEXT("/Game/Blueprints/Characters/BP_RogueCharacter"));

	if (PaladinBP_Getter.Class != NULL)
	{
		pPaladinBP = PaladinBP_Getter.Class;
	}

	if (RogueBP_Getter.Class != NULL)
	{
		pRogueBP = RogueBP_Getter.Class;
	}

	// Set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	bStartPlayersAsSpectators = true;

}

// Called when the game starts or when spawned
void ATempCapstoneProjectGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::CreatePlayer(GetWorld(), -1);

	// Acquire pointer to Viewport client
	GameViewportClient = Cast<USplitScreenGameViewportClient>(GetWorld()->GetGameViewport());
}

/// <summary>
/// Intercept players PostLogin, wait until level has loaded before spawning their proper character based on their controller.
/// </summary>
void ATempCapstoneProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	PlayerCount++;

	//  In case we want to specify custom controller classes:
	
	//	if (NewPlayer->GetLocalRole() == ROLE_Authority)
	//	{
	//		PlayerControllerClass = PlayerOneIsPaladin?pPaladinBPcontroller:pRogueBPcontroller;
	//	}
	//	if (NewPlayer->GetRemoteRole() < ROLE_Authority)
	//	{
	//		PlayerControllerClass = PlayerOneIsPaladin?pRogueBPcontroller:pPaladinBPcontroller;
	//	}

	PlayerControllerList.Add(NewPlayer);
	NewPlayer->bBlockInput = true;

	Super::PostLogin(NewPlayer);

	if (!bLevelHasLoaded)
	{
		DelaySpawnUntilLevelLoaded();
	}
	else
	{
		SpawnPawnAndPosess(NewPlayer);
	}
}

/// <summary>
/// Check if the level has loaded every half a second. If it has, spawn every player contoller submitted.
/// </summary>
void ATempCapstoneProjectGameMode::DelaySpawnUntilLevelLoaded()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Checking level loaded to spawn."));

	TArray<ULevelStreaming*> levels = GetWorld()->GetStreamingLevels();
	int LoadedCount = 0;

	if (levels.Num() != 0)
	{
		for (size_t i = 0; i < GetWorld()->GetStreamingLevels().Num(); i++)
		{
			if (levels[i]->GetCurrentState() == ULevelStreaming::ECurrentState::LoadedVisible)
				LoadedCount += 1;
		}
	}

	bLevelHasLoaded = LoadedCount == GetWorld()->GetStreamingLevels().Num();

	if (bLevelHasLoaded)
	{
		// Spawn all shade players.
		for (size_t i = 0; i < PlayerControllerList.Num(); i++)
		{
			SpawnPawnAndPosess(PlayerControllerList[i]);
		}

		GetWorldTimerManager().ClearTimer(SpawnDelayHandle);

		return;
	}

	GetWorldTimerManager().SetTimer(SpawnDelayHandle, this, &ATempCapstoneProjectGameMode::DelaySpawnUntilLevelLoaded, 0.5f, false);
}

/// <summary>
/// Helper function to spawn and posess the correct pawn based on the player's assigned controller.
/// </summary>
void ATempCapstoneProjectGameMode::SpawnPawnAndPosess(APlayerController* NewPlayer)
{
	///shadowstalk code dump
	//	if (ASTK_EntityCharacterMonsterController* monsterController = dynamic_cast<ASTK_EntityCharacterMonsterController*>(NewPlayer))
	//	{
	//		APawn* oldPawn = monsterController->GetPawnOrSpectator();
	//	
	//		monsterController->UnPossess();
	//		
	//		DefaultPawnClass = pMonsterBP;
	//		RestartPlayer(monsterController);
	//	
	//		if (oldPawn)
	//			oldPawn->Destroy();
	//	
	//		monsterController->bBlockInput = false;
	//	}
	//	else if (ASTK_EntityCharacterShadeController* shadeController = dynamic_cast<ASTK_EntityCharacterShadeController*>(NewPlayer))
	//	{
	//		APawn* oldPawn = shadeController->GetPawnOrSpectator();
	//	
	//		shadeController->UnPossess();
	//	
	//		DefaultPawnClass = pShadeBP;
	//		RestartPlayer(shadeController);
	//	
	//		if (oldPawn)
	//			oldPawn->Destroy();
	//	
	//		shadeController->bBlockInput = false;
	//	}

	APawn* oldPawn = NewPlayer->GetPawnOrSpectator();
	NewPlayer->UnPossess();

	// hacking in player pawn selection
	DefaultPawnClass =
	PlayerCount == 1 ?
	PlayerOneIsPaladin ? pPaladinBP : pRogueBP :
	PlayerOneIsPaladin ? pRogueBP : pPaladinBP;

	RestartPlayer(NewPlayer);
	
	if (oldPawn)
	oldPawn->Destroy();
}


void ATempCapstoneProjectGameMode::SetSplitscreenBias(float TargetBias, float TransitionDuration, EScreenDividerMovementStyle TransitionStyle)
{
	if (GameViewportClient != nullptr)
	{
		GameViewportClient->SetSplitscreenBias(TargetBias, TransitionDuration, TransitionStyle);
	}
}
