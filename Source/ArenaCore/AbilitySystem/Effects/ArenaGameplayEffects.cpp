// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/Effects/ArenaGameplayEffects.h"
#include "AbilitySystem/Executions/ArenaDamageExecution.h"
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
