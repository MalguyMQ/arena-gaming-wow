// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ArenaGameplayAbility.h"
#include "ArenaAbility_InstantDamage.generated.h"

// Verbe générique : dégâts instantanés sur la cible courante.
// Sert tous les sorts CSV instantanés à dégâts directs (Heurtoir, Frappe
// brutale, Lance de glace, Attaque sournoise…). Les effets secondaires
// (debuffs, ralentissements, combo) arrivent aux jalons M3/M4.
UCLASS()
class ARENACORE_API UArenaAbility_InstantDamage : public UArenaGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};
