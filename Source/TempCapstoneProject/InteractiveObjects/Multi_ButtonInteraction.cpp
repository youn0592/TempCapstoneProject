// Fill out your copyright notice in the Description page of Project Settings.


#include "Multi_ButtonInteraction.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "ButtonInteraction.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMulti_ButtonInteraction::AMulti_ButtonInteraction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>("Scene Root");
	RootComponent = SceneRoot;

	ButtonCollider = CreateDefaultSubobject<UBoxComponent>("Collider");
	ButtonCollider->SetupAttachment(RootComponent);

	ObjectMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Object Mesh");
	ObjectMesh->SetupAttachment(RootComponent);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>("Interaction Widget");
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetVisibility(false);

}

// Called when the game starts or when spawned
void AMulti_ButtonInteraction::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMulti_ButtonInteraction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMulti_ButtonInteraction::Interact()
{
	NMC_Interact();
}

void AMulti_ButtonInteraction::NMC_Interact_Implementation()
{
	// Toggle Active State
	bIsActive = !bIsActive;
	ReceiveInteract();
}

void AMulti_ButtonInteraction::ShowInteractionWidget()
{
	InteractionWidget->SetVisibility(true);
}

void AMulti_ButtonInteraction::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}

void AMulti_ButtonInteraction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMulti_ButtonInteraction, bIsActive);
}
