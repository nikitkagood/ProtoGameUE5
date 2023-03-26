// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProtoGame : ModuleRules
{
	public ProtoGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		MinFilesUsingPrecompiledHeaderOverride = 0;
		CppStandard = CppStandardVersion.Cpp17;

		bEnforceIWYU = true;

		PublicIncludePaths.Add("../Source/ProtoGame/");
	}
}

//Config/UnrealBuildTool also has settings
