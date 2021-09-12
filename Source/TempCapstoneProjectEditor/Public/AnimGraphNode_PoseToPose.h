// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AnimGraphNode_Base.h"
#include "Animation/AnimNode_PoseToPose.h"

#include "AnimGraphNode_PoseToPose.generated.h"
/**
 * 
 */
UCLASS()
class TEMPCAPSTONEPROJECTEDITOR_API UAnimGraphNode_PoseToPose : public UAnimGraphNode_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Settings)
		FAnimNode_PoseToPose BlendNode;

	//~ Begin UEdGraphNode Interface.
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UEdGraphNode Interface.

	/// virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	
	//~ Begin UAnimGraphNode_Base Interface
	virtual FString GetNodeCategory() const override;
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//~ End UAnimGraphNode_Base Interface
	
	UAnimGraphNode_PoseToPose(const FObjectInitializer& ObjectInitializer);
 
protected:

	int32 RemovedPinArrayIndex;

	// removes removed pins and adjusts array indices of remained pins
	///void RemovePinsFromOldPins(TArray<UEdGraphPin*>& OldPins, int32 RemovedArrayIndex);
	///int32 RemovedPinArrayIndex;
};