// Copyright Flagcat Studio. All Rights Reserved.

// Cible serveur dédié : compile uniquement avec un moteur buildé depuis les sources.
// Committée dès le jour 1 pour garder le code propre côté serveur (discipline WITH_SERVER_CODE).
// Le MVP tourne en listen server — ne pas builder cette cible avec un moteur du launcher.

using UnrealBuildTool;
using System.Collections.Generic;

public class ArenaGameServerTarget : TargetRules
{
	public ArenaGameServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ArenaCore");
	}
}
