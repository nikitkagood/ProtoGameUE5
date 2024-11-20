// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProtoGame : ModuleRules
{
	public ProtoGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ChaosVehicles", "AIModule", "Mover" });

		MinFilesUsingPrecompiledHeaderOverride = 0;
		CppStandard = CppStandardVersion.Latest;

		IWYUSupport = IWYUSupport.Full;

        PublicIncludePaths.Add("../Source/ProtoGame/");

		//It's in project Properties - NMake - Include Search Paths
        //PublicIncludePaths.Add("../Engine/Plugins/Experimental/Mover/Source/Mover/Public");
    }
}

//Config/UnrealBuildTool also has settings
