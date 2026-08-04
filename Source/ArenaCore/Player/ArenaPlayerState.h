// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ArenaPlayerState.generated.h"

UCLASS()
class ARENACORE_API AArenaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 0 ou 1 — assigné par le GameMode à la connexion, utilisé pour les portes de départ.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Arena")
	int32 TeamIndex = 0;

	// Ligne de StatTemplates.csv / Classes.csv. Changeable via Arena.SetClass (menu en M4).
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Arena")
	FName ClassId = FName("Warrior");
};
