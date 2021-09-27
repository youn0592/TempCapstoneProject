// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimGraphNode_PoseToPose.h"

#include "ToolMenus.h"
#include "GraphEditorActions.h"
#include "ScopedTransaction.h"

#include "Kismet2/BlueprintEditorUtils.h"

 #define LOCTEXT_NAMESPACE "AnimGraphNode_PoseToPose"

UAnimGraphNode_PoseToPose::UAnimGraphNode_PoseToPose(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

FString UAnimGraphNode_PoseToPose::GetNodeCategory() const
{
	// TODO - maybe rename this later
	return TEXT("#Custom");
}

FText UAnimGraphNode_PoseToPose::GetTooltipText() const
{
	return LOCTEXT("PoseToPoseAnimationCycle", "Pose To Pose Animation Cycle");
}

FText UAnimGraphNode_PoseToPose::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("PoseToPose", "Pose To Pose");
}

FLinearColor UAnimGraphNode_PoseToPose::GetNodeTitleColor() const
{
	return FLinearColor::Green;
}

void UAnimGraphNode_PoseToPose::CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const
{
	Super::CustomizePinData(Pin, SourcePropertyName, ArrayIndex);

	//	if (Pin->PinName == GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_PoseToPose, Alpha))
	//	{
	//		if (!Pin->bHidden)
	//		{
	//			Pin->PinFriendlyName = BlendNode.Alpha.GetFriendlyName(BlendNode.Alpha.GetFriendlyName(Pin->PinFriendlyName));
	//		}
	//	}
}

void UAnimGraphNode_PoseToPose::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{

	//	const FName PropertyName = (PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None);
	//	if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FAnimNode_PoseToPose, PoseCount))
	//	{
	//		BlendNode.PoseCount > 8 ? BlendNode.PoseCount = 8 : (BlendNode.PoseCount < 1 ? BlendNode.PoseCount = 1 : BlendNode.PoseCount = BlendNode.PoseCount);
	//	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UAnimGraphNode_PoseToPose::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	Super::CustomizeDetails(DetailBuilder);
}

//	void UAnimGraphNode_PoseToPose::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
//	{
//		Super::GetNodeContextMenuActions(Menu, Context);
//	
//		if (!Context->bIsDebugging)
//		{
//			FToolMenuSection& Section = Menu->AddSection("AnimGraphBlendList", NSLOCTEXT("AnimGraphNode_PoseToPose", "BlendListHeader", "BlendList"));
//			if (Context->Pin != NULL)
//			{
//				if (Context->Pin->Direction == EGPD_Input)
//				{
//					Section.AddMenuEntry(FGraphEditorCommands::Get().RemoveBlendListPin);
//				}
//			}
//			else
//			{
//				Section.AddMenuEntry(FGraphEditorCommands::Get().AddBlendListPin);
//			}
//		}
//	}

#undef LOCTEXT_NAMESPACE

