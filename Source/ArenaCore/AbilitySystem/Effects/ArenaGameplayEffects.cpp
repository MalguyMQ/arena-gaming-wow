// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/Effects/ArenaGameplayEffects.h"
#include "AbilitySystem/Executions/ArenaDamageExecution.h"
#include "AbilitySystem/Executions/ArenaHealExecution.h"
#include "ArenaGameplayTags.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UArenaGE_GlobalCooldown::UArenaGE_GlobalCooldown()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat DurationSetByCaller;
	DurationSetByCaller.DataTag = ArenaTags::Data_Duration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(DurationSetByCaller);

	UTargetTagsGameplayEffectComponent& TargetTagsComponent =
		*CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTags"));
	FInheritedTagContainer TagChanges;
	TagChanges.Added.AddTag(ArenaTags::Cooldown_GCD);
	TargetTagsComponent.SetAndApplyTargetTagChanges(TagChanges);
	GEComponents.Add(&TargetTagsComponent);
}

UArenaGE_AbilityCooldown::UArenaGE_AbilityCooldown()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat DurationSetByCaller;
	DurationSetByCaller.DataTag = ArenaTags::Data_Duration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(DurationSetByCaller);
}

UArenaGE_Damage::UArenaGE_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition Execution;
	Execution.CalculationClass = UArenaDamageExecution::StaticClass();
	Executions.Add(Execution);
}

UArenaGE_Heal::UArenaGE_Heal()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition Execution;
	Execution.CalculationClass = UArenaHealExecution::StaticClass();
	Executions.Add(Execution);
}

UArenaGE_SchoolLockout::UArenaGE_SchoolLockout()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat DurationSetByCaller;
	DurationSetByCaller.DataTag = ArenaTags::Data_Duration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(DurationSetByCaller);
}
