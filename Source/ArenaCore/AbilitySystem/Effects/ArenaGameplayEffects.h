// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "ArenaGameplayEffects.generated.h"

// GameplayEffects statiques du jeu, configurés au constructeur.
// Règle du projet : jamais de NewObject<UGameplayEffect> au runtime (ne réplique
// pas) — toute la variabilité passe par des magnitudes SetByCaller.

// GCD : durée SetByCaller (Data.Duration), calculée avec la hâte par l'ability.
UCLASS()
class ARENACORE_API UArenaGE_GlobalCooldown : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UArenaGE_GlobalCooldown();
};

// Cooldown par sort : durée SetByCaller ; le tag Cooldown.Ability.Slot<N> est
// ajouté dynamiquement sur le spec par l'ability (DynamicGrantedTags).
UCLASS()
class ARENACORE_API UArenaGE_AbilityCooldown : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UArenaGE_AbilityCooldown();
};

// Dégâts instantanés : la valeur pré-mitigation arrive en SetByCaller
// (Data.Damage), l'ExecCalc applique crit et multiplicateurs de la cible.
UCLASS()
class ARENACORE_API UArenaGE_Damage : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UArenaGE_Damage();
};

// Soins : valeur en SetByCaller (Data.Heal), ExecCalc applique crit et
// multiplicateur de soins reçus (réduction type Frappe brutale).
UCLASS()
class ARENACORE_API UArenaGE_Heal : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UArenaGE_Heal();
};

// Verrouillage d'école après interruption : durée SetByCaller (Data.Duration),
// le tag Lockout.School.<École> est ajouté dynamiquement sur le spec.
UCLASS()
class ARENACORE_API UArenaGE_SchoolLockout : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UArenaGE_SchoolLockout();
};
