// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ArenaHealExecution.generated.h"

// Pipeline de soins : base en SetByCaller (Data.Heal), jet de critique, puis
// multiplicateur de soins reçus de la cible (réduction type Frappe brutale).
// Sortie dans le méta-attribut Healing, consommé par PostGameplayEffectExecute.
UCLASS()
class ARENACORE_API UArenaHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UArenaHealExecution();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
