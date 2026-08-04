// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "System/ArenaDataRows.h"
#include "ArenaGameplayAbility.generated.h"

// Base de tous les sorts. Un sort = une ligne d'Abilities.csv : l'ability est
// donnée avec InputID = slot, et retrouve sa ligne à l'activation via le
// personnage (AbilitySlotRows). Toute la configuration vient de la ligne —
// aucune donnée dupliquée dans les classes C++.
// Gère : GCD hâté (plancher 0,75 s), cooldown par slot, vérification de portée.
UCLASS(Abstract)
class ARENACORE_API UArenaGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UArenaGameplayAbility();

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	// Ligne CSV de ce sort (nullptr si introuvable). Valide côté client et serveur.
	const FAbilityRow* GetAbilityRow(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo) const;

	// Cible courante de l'avatar, avec validation de distance (RangeM + marge capsules).
	AActor* GetValidatedTarget(
		const FAbilityRow& Row,
		const FGameplayAbilityActorInfo* ActorInfo) const;

	static constexpr float GCDBaseSeconds = 1.5f;
	static constexpr float GCDFloorSeconds = 0.75f;
	static constexpr float RangeSlackCm = 150.f;
};
