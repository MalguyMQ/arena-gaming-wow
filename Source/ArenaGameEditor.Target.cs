// Copyright Flagcat Studio. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ArenaGameEditorTarget : TargetRules
{
	public ArenaGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ArenaCore");
	}
}
