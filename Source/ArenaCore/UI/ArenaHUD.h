// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ArenaHUD.generated.h"

class UArenaMainWidget;

// Crée le widget principal du HUD pour le joueur local.
UCLASS()
class ARENACORE_API AArenaHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UArenaMainWidget> MainWidget;
};
