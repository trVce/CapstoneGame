// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Capstone2077 : ModuleRules
{
	public Capstone2077(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "Slate", "SlateCore" });
	}
}
