// Fill out your copyright notice in the Description page of Project Settings.


#include "SensorInteraction.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "TempCapstoneProject/PaladinCharacter.h"
#include "TempCapstoneProject/RogueCharacter.h"

// Sets default values
ASensorInteraction::ASensorInteraction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("Scene Root");
	RootComponent = SceneRoot;

	SensorCollider = CreateDefaultSubobject<UBoxComponent>("Sensor Collider");
	SensorCollider->SetupAttachment(RootComponent);

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh");
	ObjectMesh->SetupAttachment(RootComponent);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>("Interaction Widget");
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetVisibility(false);

	// Register overlap events
	OnActorBeginOverlap.AddDynamic(this, &ASensorInteraction::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ASensorInteraction::OnOverlapEnd);
}

void ASensorInteraction::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		if (OtherActor->IsA(APaladinCharacter::StaticClass()) ||
			OtherActor->IsA(ARogueCharacter::StaticClass()))
		{
			bIsActive = true;
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, "Overlap Begin");
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Other Actor = %s"), *OtherActor->GetName()));
		}
	}
}

void ASensorInteraction::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && OtherActor != nullptr)
	{
		if (OtherActor->IsA(APaladinCharacter::StaticClass()) ||
			OtherActor->IsA(ARogueCharacter::StaticClass()))
		{
			bIsActive = false;
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, "Overlap Ended");
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("%s has left the Sensor"), *OtherActor->GetName()));
		}
	}
}

// Called when the game starts or when spawned
void ASensorInteraction::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASensorInteraction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		Interact();
	}
}

void ASensorInteraction::Interact()
{
	ReceiveInteract();
}

void ASensorInteraction::ShowInteractionWidget()
{
    
}

void ASensorInteraction::HideInteractionWidget()
{
    
}

