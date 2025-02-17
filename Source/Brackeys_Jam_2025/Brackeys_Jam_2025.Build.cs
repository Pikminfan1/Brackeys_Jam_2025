// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Brackeys_Jam_2025 : ModuleRules
{
	public Brackeys_Jam_2025(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
