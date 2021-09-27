// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueShield.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

// Sets default values
ARogueShield::ARogueShield()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	SetReplicates(true);

	PrimaryActorTick.bCanEverTick = true;

	ShieldCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ShieldComp"));
	ShieldCollider->InitBoxExtent(FVector(40.0f, 40.0f, 2.5f));
	ShieldCollider->SetEnableGravity(false);
	ShieldCollider->SetNotifyRigidBodyCollision(true);
	ShieldCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	ShieldCollider->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SetRootComponent(ShieldCollider);

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetSimulatePhysics(false);
	ShieldMesh->SetCollisionProfileName("NoCollision");
	ShieldMesh->SetRelativeScale3D(FVector(0.02f));
	ShieldMesh->SetupAttachment(ShieldCollider);
	ShieldMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 2.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>FindShieldMesh(TEXT("/Game/Geometry/Meshes/TemplateFloor"));
	if (FindShieldMesh.Succeeded())
	{
		ShieldMesh->SetStaticMesh(FindShieldMesh.Object);
	}

	m_IsVisible = false;
	SetActorHiddenInGame(!m_IsVisible);
	SetActorEnableCollision(m_IsVisible);
	SetActorTickEnabled(m_IsVisible);
}

void ARogueShield::ToggleShield()
{
	Server_ToggleShield();
	SetActorHiddenInGame(!m_IsVisible);
	SetActorEnableCollision(m_IsVisible);
}


void ARogueShield::Server_ToggleShield_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 0.2, FColor::Yellow, FString::Printf(TEXT("ServerCalled")));
	m_IsVisible = !m_IsVisible;
}

// Called when the game starts or when spawned
void ARogueShield::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARogueShield::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARogueShield, m_IsVisible);
}