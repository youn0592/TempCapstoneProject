 // Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class TempCapstoneProjectEditor : ModuleRules
{
	public TempCapstoneProjectEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "TempCapstoneProject", "BlueprintGraph"});
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		PrivateDependencyModuleNames.AddRange(new string[] { "AnimGraph", "UnrealEd", "ToolMenus", "SlateCore", "GraphEditor" });

		PublicIncludePaths.AddRange(new string[] { "TempCapstoneProject/" });

		OverridePackageType = PackageOverrideType.GameUncookedOnly;

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
