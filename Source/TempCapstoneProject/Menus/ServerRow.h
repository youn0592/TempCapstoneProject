// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class TEMPCAPSTONEPROJECT_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

public: 
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostUser;

	UPROPERTY(BlueprintReadOnly)
	bool isSelected = false;

	void Setup(class UMainMenu* InParent, uint32 InIndex);

private: 

	UPROPERTY(meta = (BindWidget))
	class UButton* RowButton;

	UPROPERTY(meta = (BindWidget))
	class UMainMenu* Parent;

	uint32 Index;

	UFUNCTION()
	void OnClicked();
};
