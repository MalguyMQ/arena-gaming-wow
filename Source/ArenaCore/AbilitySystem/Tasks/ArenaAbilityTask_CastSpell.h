// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ArenaAbilityTask_CastSpell.generated.h"

// Minuterie d'incantation : tick jusqu'à la durée demandée, annule si le
// personnage bouge (comportement WoW). Tourne sur le serveur ET sur le client
// qui prédit — le serveur reste autoritaire sur l'effet final du sort.
UCLASS()
class ARENACORE_API UArenaAbilityTask_CastSpell : public UAbilityTask
{
	GENERATED_BODY()

public:
	static UArenaAbilityTask_CastSpell* CastSpell(UGameplayAbility* OwningAbility, float InDuration, bool bInCancelOnMove);

	// C++ uniquement — pas de dynamic delegates nécessaires.
	FSimpleMulticastDelegate OnCastFinished;
	FSimpleMulticastDelegate OnCastCancelled;

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

private:
	float Duration = 0.f;
	float Elapsed = 0.f;
	bool bCancelOnMove = true;
	bool bDone = false;

	static constexpr float MoveCancelSpeed = 50.f;
};
