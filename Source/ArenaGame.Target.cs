// Copyright Flagcat Studio. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ArenaGameTarget : TargetRules
{
	public ArenaGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ArenaCore");
	}
}
