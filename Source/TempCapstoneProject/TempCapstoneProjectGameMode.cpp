// Copyright Epic Games, Inc. All Rights Reserved.

#include "TempCapstoneProjectGameMode.h"
#include "TempCapstoneProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "SplitScreenGameViewportClient.h"
#include "Kismet/GameplayStatics.h"

// Called when the game starts or when spawned
void ATempCapstoneProjectGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::CreatePlayer(GetWorld(), -1);

	// Acquire pointer to Viewport client
	GameViewportClient = Cast<USplitScreenGameViewportClient>(GetWorld()->GetGameViewport());
}

ATempCapstoneProjectGameMode::ATempCapstoneProjectGameMode()
{
	// Set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ATempCapstoneProjectGameMode::SetSplitscreenBias(float TargetBias, float TransitionDuration, EScreenDividerMovementStyle TransitionStyle)
{
	if (GameViewportClient != nullptr)
	{
		GameViewportClient->SetSplitscreenBias(TargetBias, TransitionDuration, TransitionStyle);
	}
}