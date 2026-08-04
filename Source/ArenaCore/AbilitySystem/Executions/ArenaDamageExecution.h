// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ArenaDamageExecution.generated.h"

// Pipeline de dégâts : base pré-mitigation en SetByCaller (Data.Damage),
// jet de critique (CritPct source, snapshot au lancer) puis multiplicateur
// de dégâts subis de la cible. Sortie dans le méta-attribut Damage, consommé
// par PostGameplayEffectExecute (absorption → PV → feedback).
UCLASS()
class ARENACORE_API UArenaDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UArenaDamageExecution();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
