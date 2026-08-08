// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ArenaGameplayAbility.h"
#include "ArenaAbility_Interrupt.generated.h"

// Verbe générique : interruption (Fracasse-crâne, Contresort, Coup de pied).
// Hors GCD. Si la cible incante : annule le cast et verrouille l'école du sort
// interrompu pendant MiscValue secondes. Sur une cible qui n'incante pas :
// le cooldown est dépensé pour rien, exactement comme sur WoW.
UCLASS()
class ARENACORE_API UArenaAbility_Interrupt : public UArenaGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};
