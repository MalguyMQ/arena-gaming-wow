// Copyright Flagcat Studio. All Rights Reserved.

#include "System/ArenaAssetManager.h"
#include "ArenaCore.h"
#include "AbilitySystemGlobals.h"

UArenaAssetManager& UArenaAssetManager::Get()
{
	UArenaAssetManager* Singleton = Cast<UArenaAssetManager>(GEngine->AssetManager);
	if (!Singleton)
	{
		UE_LOG(LogArena, Fatal, TEXT("AssetManagerClassName doit valoir /Script/ArenaCore.ArenaAssetManager dans DefaultEngine.ini"));
	}
	return *Singleton;
}

void UArenaAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
	UE_LOG(LogArena, Log, TEXT("GAS InitGlobalData effectué."));
}
