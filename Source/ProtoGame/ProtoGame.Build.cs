// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProtoGame : ModuleRules
{
	public ProtoGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ChaosVehicles", "AIModule" });

		MinFilesUsingPrecompiledHeaderOverride = 0;
		CppStandard = CppStandardVersion.Cpp17;

		IWYUSupport = IWYUSupport.Full;

        PublicIncludePaths.Add("../Source/ProtoGame/");
	}
}

//Config/UnrealBuildTool also has settings
