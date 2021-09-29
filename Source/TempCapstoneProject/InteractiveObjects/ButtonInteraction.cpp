// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonInteraction.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AButtonInteraction::AButtonInteraction()
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
void AButtonInteraction::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AButtonInteraction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AButtonInteraction::Interact()
{
	NMC_Interact();
}

void AButtonInteraction::NMC_Interact_Implementation()
{
	// Toggle Active State
	bIsActive = !bIsActive;
	ReceiveInteract();
}

void AButtonInteraction::ShowInteractionWidget()
{
	InteractionWidget->SetVisibility(true);
}

void AButtonInteraction::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}

void AButtonInteraction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AButtonInteraction, bIsActive);
	DOREPLIFETIME(AButtonInteraction, TargetActor);
}
