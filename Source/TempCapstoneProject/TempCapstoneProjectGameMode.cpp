// Copyright Epic Games, Inc. All Rights Reserved.

#include "TempCapstoneProjectGameMode.h"
#include "TempCapstoneProjectCharacter.h"
#include "DummyPawn.h"

#include "UObject/ConstructorHelpers.h"

#include "SplitScreenGameViewportClient.h"
#include "Engine/LevelStreaming.h"

#include "Kismet/GameplayStatics.h"

ATempCapstoneProjectGameMode::ATempCapstoneProjectGameMode()
{
	// Get all our required bps and controllers
	static ConstructorHelpers::FClassFinder<ACharacter> PaladinBP_Getter(TEXT("/Game/Blueprints/Characters/BP_PaladinCharacter"));
	static ConstructorHelpers::FClassFinder<ACharacter> RogueBP_Getter(TEXT("/Game/Blueprints/Characters/BP_RogueCharacter"));
	static ConstructorHelpers::FClassFinder<APawn> DummyLocalBP_Getter(TEXT("/Game/Blueprints/Characters/BP_DummyPawn"));

	if (DummyLocalBP_Getter.Class != NULL)
	{
		pDummyBP = DummyLocalBP_Getter.Class;
	}

	if (PaladinBP_Getter.Class != NULL)
	{
		pPaladinBP = PaladinBP_Getter.Class;
	}

	if (RogueBP_Getter.Class != NULL)
	{
		pRogueBP = RogueBP_Getter.Class;
	}

	bStartPlayersAsSpectators = true;
}

// Called when the game starts or when spawned
void ATempCapstoneProjectGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Acquire pointer to Viewport client
	GameViewportClient = Cast<USplitScreenGameViewportClient>(GetWorld()->GetGameViewport());
}

/// <summary>
/// Intercept players PostLogin, wait until level has loaded before spawning their proper character based on their controller.
/// </summary>
void ATempCapstoneProjectGameMode::PostLogin(APlayerController* NewPlayer)
{

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
/// Helper function to spawn and posess the correct pawn based on the player's assigned controller. Also spawns a dummy pawn for Splitscreen purposes
/// </summary>
void ATempCapstoneProjectGameMode::SpawnPawnAndPosess(APlayerController* NewPlayer)
{
	PlayerCount++;

	APawn* oldPawn = NewPlayer->GetPawnOrSpectator();
	NewPlayer->UnPossess();

	//	Oh god oh fuck
	switch (PlayerCount)
	{
	case 1:
		DefaultPawnClass = PlayerOneIsPaladin ? pPaladinBP : pRogueBP;
		RestartPlayer(NewPlayer);
		break;
	
	case 2:
		DefaultPawnClass = pDummyBP;
		RestartPlayer(NewPlayer);
		break;
	
	case 3:
		DefaultPawnClass = PlayerOneIsPaladin ? pRogueBP : pPaladinBP;
		RestartPlayer(NewPlayer);
		break;
	
	case 4:
		DefaultPawnClass = pDummyBP;
		RestartPlayer(NewPlayer);

		// everyone's in the game, setup dummy pawns
		Cast<ADummyPawn>(PlayerControllerList[1]->GetPawn())->SetupDummyPawn( Cast<ATempCapstoneProjectCharacter>(PlayerControllerList[2]->GetCharacter()) );
		Cast<ADummyPawn>(PlayerControllerList[3]->GetPawn())->SetupDummyPawn( Cast<ATempCapstoneProjectCharacter>(PlayerControllerList[0]->GetCharacter()) );

		break;
	}


	//	if (oldPawn)
	//	oldPawn->Destroy();
	//	/// Spawn Player
	//	{
	//		APawn* oldPawn = NewPlayer->GetPawnOrSpectator();
	//		NewPlayer->UnPossess();
	//	
	//		// hacking in player pawn selection
	//		DefaultPawnClass = 
	//			PlayerCount == 1 ?
	//			PlayerOneIsPaladin ? pPaladinBP : pRogueBP:
	//			PlayerOneIsPaladin ? pRogueBP : pPaladinBP;
	//	
	//		RestartPlayer(NewPlayer);
	//		
	//		// FString s;
	//		// ULocalPlayer* x = GetGameInstance()->CreateLocalPlayer(PlayerCount + 2, s, false);
	//		// RestartPlayer(x->GetPlayerController(GetWorld()));
	//	
	//		if (oldPawn)
	//			oldPawn->Destroy();
	//	}
	
	// workaround for prototype: grab spawned character and possess  

}


void ATempCapstoneProjectGameMode::SetSplitscreenBias(float TargetBias, float TransitionDuration, EScreenDividerMovementStyle TransitionStyle)
{
	if (GameViewportClient != nullptr)
	{
		GameViewportClient->SetSplitscreenBias(TargetBias, TransitionDuration, TransitionStyle);
	}
}


// CODE DUMP 

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
//

	/// Spawn Dummy Pawn
	//	{
		// ATempCapstoneProjectCharacter* NewCharacter = Cast<ATempCapstoneProjectCharacter>(NewPlayer->GetCharacter());

//		FActorSpawnParameters params;
//		params.bNoFail = true;
//		params.Owner = NewCharacter;
//
//		UClass* PawnClass = pDummyBP->GetClass();
//		ADummyPawn* DummyPawn = GetWorld()->SpawnActor<ADummyPawn>(NewCharacter->GetActorLocation(), NewCharacter->GetActorRotation(), params);
		// ADummyPawn* DummyPawn = GetWorld()->SpawnActor<ADummyPawn>(PawnClass, &NewCharacter->GetTransform(), params);
		// ADummyPawn* DummyPawn = Cast<ADummyPawn>(GetWorld()->SpawnActor(pDummyBP->GetClass(), &NewCharacter->GetTransform(), params));
		// DefaultPawnClass = pDummyBP;
		// FString ErrorMsg;
		// ULocalPlayer* Dummy = GetWorld()->GetGameInstance()->CreateLocalPlayer(PlayerCount + 2, ErrorMsg, true);
		// Dummy->GetPlayerController(GetWorld())->Possess(DummyPawn);
		// DummyPawn->SetupDummyPawn(NewCharacter);
		// This calls SetupDummyPawn on a DummyPawn actor that gets spawned, passing in the new character.
		// Cast<ADummyPawn>(UGameplayStatics::CreatePlayer(GetWorld(), -1, true)->GetPawn())->SetupDummyPawn(Cast<ATempCapstoneProjectCharacter>(NewPlayer->GetCharacter()));
	//	}

/*	if (DefaultPawnClass == pDummyBP)
	{
		APawn* oldPawn = NewPlayer->GetPawnOrSpectator();
		NewPlayer->UnPossess();
		RestartPlayer(NewPlayer);
		if (oldPawn)
			oldPawn->Destroy();

		DefaultPawnClass = pPaladinBP;
	}
	else 
	{
		PlayerCount++;

		// hacking in player pawn selection
		DefaultPawnClass =
			PlayerCount == 1 ?
			PlayerOneIsPaladin ? pPaladinBP : pRogueBP :
			PlayerOneIsPaladin ? pRogueBP : pPaladinBP;

		/// Spawn Player
		APawn* oldPawn = NewPlayer->GetPawnOrSpectator();
		NewPlayer->UnPossess();
		RestartPlayer(NewPlayer);
		if (oldPawn)
			oldPawn->Destroy();

		DefaultPawnClass = pDummyBP;
		UGameplayStatics::CreatePlayer(GetWorld(), PlayerCount + 2);

	}*/