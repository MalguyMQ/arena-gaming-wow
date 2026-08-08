// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "System/ArenaDataRows.h"
#include "ArenaGameplayAbility.generated.h"

class AArenaCharacter;

// Base de tous les sorts. Un sort = une ligne d'Abilities.csv : l'ability est
// donnée avec InputID = slot, et retrouve sa ligne à l'activation via le
// personnage (AbilitySlotRows). Toute la configuration vient de la ligne.
// Gère : GCD hâté (plancher 0,75 s), cooldown par slot, coût en ressource
// (mana/rage/énergie), verrouillage d'école, portée, orientation, ligne de vue.
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

	virtual bool CheckCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	// Vrai si A et B sont des joueurs de la même équipe (les mannequins ne sont jamais alliés).
	static bool AreAllies(const AArenaCharacter* A, const AArenaCharacter* B);

	// Trace de visibilité entre deux acteurs (les piliers bloquent).
	static bool HasLineOfSight(const AActor* From, const AActor* To);

protected:
	// Ligne CSV de ce sort (nullptr si introuvable). Valide côté client et serveur.
	const FAbilityRow* GetAbilityRow(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo) const;

	// Résout et valide la cible selon la ligne : allié-ou-soi pour les sorts
	// alliés, cible courante sinon ; portée, orientation (bRequiresFacing) et
	// ligne de vue (bRequiresLoS, si bCheckLoS). nullptr si invalide.
	AActor* GetValidatedTarget(
		const FAbilityRow& Row,
		const FGameplayAbilityActorInfo* ActorInfo,
		bool bCheckLoS = true) const;

	static FGameplayAttribute ResourceAttributeFromName(FName ResourceType);
	static float GetResourceCurrent(const UAbilitySystemComponent* ASC, FName ResourceType);

	static constexpr float GCDBaseSeconds = 1.5f;
	static constexpr float GCDFloorSeconds = 0.75f;
	static constexpr float RangeSlackCm = 150.f;
};
