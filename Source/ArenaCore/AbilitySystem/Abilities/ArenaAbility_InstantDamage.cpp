// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/Abilities/ArenaAbility_InstantDamage.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "AbilitySystem/Effects/ArenaGameplayEffects.h"
#include "ArenaGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UArenaAbility_InstantDamage::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	const FAbilityRow* Row = GetAbilityRow(Handle, ActorInfo);
	if (!Row)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* Target = GetValidatedTarget(*Row, ActorInfo);
	if (!Target)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (HasAuthority(&ActivationInfo))
	{
		UAbilitySystemComponent* SourceASC = ActorInfo->AbilitySystemComponent.Get();
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
		if (SourceASC && TargetASC)
		{
			const float AttackPower = SourceASC->GetNumericAttribute(UArenaAttributeSet::GetAttackPowerAttribute());
			const float SpellPower = SourceASC->GetNumericAttribute(UArenaAttributeSet::GetSpellPowerAttribute());
			const float PreMitigationDamage =
				Row->DamageBase + AttackPower * Row->DamageAPCoef + SpellPower * Row->DamageSPCoef;

			FGameplayEffectSpecHandle DamageSpec = MakeOutgoingGameplayEffectSpec(
				Handle, ActorInfo, ActivationInfo, UArenaGE_Damage::StaticClass(), 1.f);
			if (DamageSpec.IsValid() && PreMitigationDamage > 0.f)
			{
				DamageSpec.Data->SetSetByCallerMagnitude(ArenaTags::Data_Damage, PreMitigationDamage);
				SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data.Get(), TargetASC);
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
