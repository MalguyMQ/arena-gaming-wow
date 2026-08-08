// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ArenaAbilitySystemComponent.generated.h"

// ASC du jeu, avec la spell queue window à la WoW : un input pressé dans les
// ~400 dernières ms d'un GCD, d'un cooldown ou d'une incantation est mis en
// file et part dès que le blocage expire. C'est ce qui rend les enchaînements
// fluides sous latence — GAS ne prédit pas l'expiration des cooldowns, on ne
// se bat pas contre ça.
UCLASS()
class ARENACORE_API UArenaAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UArenaAbilitySystemComponent();

	// Point d'entrée des touches de la barre d'action (client local).
	void TryActivateSlot(int32 Slot);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Arena")
	float SpellQueueWindowSec = 0.4f;

private:
	// Temps restant avant que le slot soit activable : max(GCD, cooldown du slot, incantation en cours).
	float GetBlockedRemainingForSlot(int32 Slot) const;

	int32 QueuedSlot = INDEX_NONE;
	double QueueExpireTime = 0.0;
};
