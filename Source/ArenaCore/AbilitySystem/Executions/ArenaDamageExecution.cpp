// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/Executions/ArenaDamageExecution.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "ArenaGameplayTags.h"

namespace
{
	struct FArenaDamageStatics
	{
		DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTakenMult);
		DECLARE_ATTRIBUTE_CAPTUREDEF(CritPct);

		FArenaDamageStatics()
		{
			DEFINE_ATTRIBUTE_CAPTUREDEF(UArenaAttributeSet, DamageTakenMult, Target, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UArenaAttributeSet, CritPct, Source, true);
		}
	};

	const FArenaDamageStatics& DamageStatics()
	{
		static FArenaDamageStatics Statics;
		return Statics;
	}

	constexpr float CritMultiplier = 2.f;
}

UArenaDamageExecution::UArenaDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageTakenMultDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritPctDef);
}

void UArenaDamageExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	const float BaseDamage = Spec.GetSetByCallerMagnitude(ArenaTags::Data_Damage, false, 0.f);
	if (BaseDamage <= 0.f)
	{
		return;
	}

	float DamageTakenMult = 1.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().DamageTakenMultDef, EvalParams, DamageTakenMult);

	float CritChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CritPctDef, EvalParams, CritChance);

	float FinalDamage = BaseDamage;
	if (FMath::FRandRange(0.f, 100.f) < CritChance)
	{
		FinalDamage *= CritMultiplier;
	}
	FinalDamage *= FMath::Max(0.f, DamageTakenMult);

	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UArenaAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}
}
