// Copyright Flagcat Studio. All Rights Reserved.

using UnrealBuildTool;

public class ArenaCore : ModuleRules
{
	public ArenaCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Les includes du module sont écrits relatifs à sa racine
		// (ex. "System/ArenaDataRows.h") — UBT ne l'ajoute pas tout seul
		// quand les sources sont rangées en sous-dossiers.
		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"UMG",
			"Slate",
			"SlateCore",
			"NetCore"
		});
	}
}
