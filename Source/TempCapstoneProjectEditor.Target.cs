// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TempCapstoneProjectEditorTarget : TargetRules
{
	public TempCapstoneProjectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("TempCapstoneProject");
		ExtraModuleNames.Add("TempCapstoneProjectEditor");
	}
}
