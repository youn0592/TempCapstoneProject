// Fill out your copyright notice in the Description page of Project Settings.


#include "RogueShield.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

// Sets default values
ARogueShield::ARogueShield()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

	ShieldEnable(false);

	SetReplicates(true);
}

void ARogueShield::ShieldEnable(bool e)
{
	Server_ShieldEnable(e);

	// Works on client but not on Server. Tried from both sides.
	//SetActorHiddenInGame(!e); // Actor is being Replicated
	//SetActorEnableCollision(e);
	//SetActorTickEnabled(e);
	//ShieldMesh->SetVisibility(e); // Mesh is being Replicated
}

void ARogueShield::Server_ShieldEnable_Implementation(bool e)
{
	// Works on client but not on Server. Tried from both sides.
	SetActorHiddenInGame(!e); // Actor is being Replicated
	SetActorEnableCollision(e);
	SetActorTickEnabled(e);
	ShieldMesh->SetHiddenInGame(!e, true); // Mesh is being Replicated	
}

// Called when the game starts or when spawned
void ARogueShield::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARogueShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void ARogueShield::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	/*DOREPLIFETIME(ARogueShield, ShieldMesh);
//	DOREPLIFETIME(ARogueShield, m_IsVisible);*/
//}