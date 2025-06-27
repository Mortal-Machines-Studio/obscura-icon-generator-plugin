// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ObscuraIconGeneratorEditor : ModuleRules
{
	public ObscuraIconGeneratorEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "UMG", "UMGEditor", "DeveloperSettings",
				"ScriptableEditorWidgets", "InputCore", "Projects", "UnrealEd", "Blutility", 
				"ToolMenus", "ContentBrowser"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Blutility",
				"UMG", 
				"EditorScriptingUtilities"
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
