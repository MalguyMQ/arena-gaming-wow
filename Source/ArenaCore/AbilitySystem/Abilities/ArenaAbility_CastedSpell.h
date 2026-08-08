// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ArenaGameplayAbility.h"
#include "ArenaAbility_CastedSpell.generated.h"

// Verbe générique : sort à incantation (dégâts ou soins selon la ligne CSV).
// Comportement WoW : GCD et cooldown au DÉBUT du cast, coût payé à la FIN,
// bouger annule, la ligne de vue est re-vérifiée à la fin (le juke derrière
// un pilier fait échouer le sort). Interruptible pendant toute l'incantation.
UCLASS()
class ARENACORE_API UArenaAbility_CastedSpell : public UArenaGameplayAbility
{
	GENERATED_BODY()

public:
	UArenaAbility_CastedSpell();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:
	void OnCastFinished();
	void OnCastCancelled();

	TWeakObjectPtr<AActor> LockedTarget;
};
