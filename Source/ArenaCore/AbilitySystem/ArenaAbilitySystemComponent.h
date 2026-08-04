// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ArenaAbilitySystemComponent.generated.h"

// ASC du jeu. M3 y ajoutera la spell queue window (~400 ms) et le routage
// d'input avancé ; pour l'instant il porte la configuration commune.
UCLASS()
class ARENACORE_API UArenaAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UArenaAbilitySystemComponent();
};
