// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/Executions/ArenaHealExecution.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "ArenaGameplayTags.h"

namespace
{
	struct FArenaHealStatics
	{
		DECLARE_ATTRIBUTE_CAPTUREDEF(HealingTakenMult);
		DECLARE_ATTRIBUTE_CAPTUREDEF(CritPct);

		FArenaHealStatics()
		{
			DEFINE_ATTRIBUTE_CAPTUREDEF(UArenaAttributeSet, HealingTakenMult, Target, false);
			DEFINE_ATTRIBUTE_CAPTUREDEF(UArenaAttributeSet, CritPct, Source, true);
		}
	};

	const FArenaHealStatics& HealStatics()
	{
		static FArenaHealStatics Statics;
		return Statics;
	}

	constexpr float CritMultiplier = 2.f;
}

UArenaHealExecution::UArenaHealExecution()
{
	RelevantAttributesToCapture.Add(HealStatics().HealingTakenMultDef);
	RelevantAttributesToCapture.Add(HealStatics().CritPctDef);
}

void UArenaHealExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	const float BaseHeal = Spec.GetSetByCallerMagnitude(ArenaTags::Data_Heal, false, 0.f);
	if (BaseHeal <= 0.f)
	{
		return;
	}

	float HealingTakenMult = 1.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		HealStatics().HealingTakenMultDef, EvalParams, HealingTakenMult);

	float CritChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		HealStatics().CritPctDef, EvalParams, CritChance);

	float FinalHeal = BaseHeal;
	if (FMath::FRandRange(0.f, 100.f) < CritChance)
	{
		FinalHeal *= CritMultiplier;
	}
	FinalHeal *= FMath::Max(0.f, HealingTakenMult);

	if (FinalHeal > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UArenaAttributeSet::GetHealingAttribute(), EGameplayModOp::Additive, FinalHeal));
	}
}
