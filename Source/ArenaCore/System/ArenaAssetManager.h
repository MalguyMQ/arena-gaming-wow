// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ArenaAssetManager.generated.h"

// AssetManager custom : indispensable pour initialiser les données globales de GAS
// (UAbilitySystemGlobals::InitGlobalData) — sans ça, la réplication de TargetData
// échoue silencieusement. Câblé via AssetManagerClassName dans DefaultEngine.ini.
UCLASS()
class ARENACORE_API UArenaAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UArenaAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
