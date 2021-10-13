// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TempCapstoneProject : ModuleRules
{
	public TempCapstoneProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemNull", "Steamworks","Json","JsonUtilities"});
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}
