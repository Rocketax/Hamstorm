// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Hamstorm : ModuleRules
{
	public Hamstorm(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"AIModule",
			"NavigationSystem",
			"Niagara",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils" });
		
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Steam", "lib", "steam_api64.lib"));
		
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
