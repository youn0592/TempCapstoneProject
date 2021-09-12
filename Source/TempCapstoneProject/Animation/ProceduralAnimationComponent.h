// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralAnimationComponent.generated.h"

class UCapsuleComponent;

USTRUCT(BlueprintType)
struct TEMPCAPSTONEPROJECT_API FCharacterAnimInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
		float StrideLength = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|IK")
		FVector Lean;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|IK")
		FVector RightFootIK_Target;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|IK")
		FVector LeftFootIK_Target;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEMPCAPSTONEPROJECT_API UProceduralAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UProceduralAnimationComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation Component")
		class USkeletalMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation Component")
		FCharacterAnimInfo AnimInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation Component")
		class UStaticMeshComponent* WheelMesh;

protected:

	UCapsuleComponent* CapsuleCollider = nullptr;
	virtual void BeginPlay() override;

public:	
	
	void Setup(UCapsuleComponent* CharacterCapsule);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
