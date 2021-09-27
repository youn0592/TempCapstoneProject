// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TempCapstoneProject.h"
#include "TempCapstoneProjectCharacter.generated.h"

UCLASS(config=Game)
class ATempCapstoneProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:

	ATempCapstoneProjectCharacter();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Animation Component")
	class UProceduralAnimationComponent* ProcAnimComp;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UProceduralAnimationComponent* GetProceduralAnimComponent() { return ProcAnimComp; };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

public:

	virtual void BeginPlay() override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	UFUNCTION(BlueprintCallable)
	virtual ECharacterType GetCharacterType() { return ECharacterType::NONE; }

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* InteractionBox;

	class IInteractionInterface* Interface = nullptr;

	void OnInteract();

	UFUNCTION(Server, Reliable)
	void Server_OnInteract();


	void CheckClosestInteraction();
};

