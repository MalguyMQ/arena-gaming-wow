// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/Abilities/ArenaAbility_CastedSpell.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "AbilitySystem/Effects/ArenaGameplayEffects.h"
#include "AbilitySystem/Tasks/ArenaAbilityTask_CastSpell.h"
#include "ArenaGameplayTags.h"
#include "Characters/ArenaCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UArenaAbility_CastedSpell::UArenaAbility_CastedSpell()
{
	ActivationOwnedTags.AddTag(ArenaTags::State_Casting);
}

void UArenaAbility_CastedSpell::ActivateAbility(
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

	// WoW : GCD et cooldown partent au début du cast ; le coût est payé à la fin.
	if (!CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false, nullptr))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	LockedTarget = Target;

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	const float Haste = ASC
		? FMath::Max(0.f, ASC->GetNumericAttribute(UArenaAttributeSet::GetHastePctAttribute())) / 100.f
		: 0.f;
	const float CastDuration = Row->CastTimeSec / (1.f + Haste);

	if (HasAuthority(&ActivationInfo))
	{
		if (AArenaCharacter* Character = Cast<AArenaCharacter>(ActorInfo->AvatarActor.Get()))
		{
			const FGameplayAbilitySpec* Spec = ASC ? ASC->FindAbilitySpecFromHandle(Handle) : nullptr;
			const FName RowName = Spec ? Character->GetAbilityRowForSlot(Spec->InputID) : NAME_None;
			Character->SetCastState(RowName, CastDuration);
		}
	}

	UArenaAbilityTask_CastSpell* Task = UArenaAbilityTask_CastSpell::CastSpell(this, CastDuration, /*bCancelOnMove*/ true);
	Task->OnCastFinished.AddUObject(this, &UArenaAbility_CastedSpell::OnCastFinished);
	Task->OnCastCancelled.AddUObject(this, &UArenaAbility_CastedSpell::OnCastCancelled);
	Task->ReadyForActivation();
}

void UArenaAbility_CastedSpell::OnCastFinished()
{
	const FGameplayAbilitySpecHandle Handle = GetCurrentAbilitySpecHandle();
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

	const FAbilityRow* Row = GetAbilityRow(Handle, ActorInfo);
	AActor* Target = LockedTarget.Get();

	if (Row && Target && HasAuthority(&ActivationInfo))
	{
		AActor* Avatar = ActorInfo->AvatarActor.Get();
		const bool bLoSOk = !Row->bRequiresLoS || HasLineOfSight(Avatar, Target);
		const bool bCostOk = CommitAbilityCost(Handle, ActorInfo, ActivationInfo, nullptr);

		if (bLoSOk && bCostOk)
		{
			UAbilitySystemComponent* SourceASC = ActorInfo->AbilitySystemComponent.Get();
			UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
			if (SourceASC && TargetASC)
			{
				const float AttackPower = SourceASC->GetNumericAttribute(UArenaAttributeSet::GetAttackPowerAttribute());
				const float SpellPower = SourceASC->GetNumericAttribute(UArenaAttributeSet::GetSpellPowerAttribute());

				if (Row->DamageBase > 0.f)
				{
					const float Damage = Row->DamageBase + AttackPower * Row->DamageAPCoef + SpellPower * Row->DamageSPCoef;
					FGameplayEffectSpecHandle DamageSpec = MakeOutgoingGameplayEffectSpec(
						Handle, ActorInfo, ActivationInfo, UArenaGE_Damage::StaticClass(), 1.f);
					if (DamageSpec.IsValid())
					{
						DamageSpec.Data->SetSetByCallerMagnitude(ArenaTags::Data_Damage, Damage);
						SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data.Get(), TargetASC);
					}
				}

				if (Row->HealBase > 0.f)
				{
					const float Heal = Row->HealBase + SpellPower * Row->HealSPCoef;
					FGameplayEffectSpecHandle HealSpec = MakeOutgoingGameplayEffectSpec(
						Handle, ActorInfo, ActivationInfo, UArenaGE_Heal::StaticClass(), 1.f);
					if (HealSpec.IsValid())
					{
						HealSpec.Data->SetSetByCallerMagnitude(ArenaTags::Data_Heal, Heal);
						SourceASC->ApplyGameplayEffectSpecToTarget(*HealSpec.Data.Get(), TargetASC);
					}
				}
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UArenaAbility_CastedSpell::OnCastCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UArenaAbility_CastedSpell::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->IsNetAuthority())
	{
		if (AArenaCharacter* Character = Cast<AArenaCharacter>(ActorInfo->AvatarActor.Get()))
		{
			Character->ClearCastState();
		}
	}
	LockedTarget = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
