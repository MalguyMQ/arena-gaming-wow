// Copyright Flagcat Studio. All Rights Reserved.

using UnrealBuildTool;

public class ArenaCore : ModuleRules
{
	public ArenaCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
