// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArenaGameState.generated.h"

// M5 : machine à phases répliquée du match (Warmup → Portes → Combat → Manche → BO3 → Victoire).
UCLASS()
class ARENACORE_API AArenaGameState : public AGameStateBase
{
	GENERATED_BODY()
};
