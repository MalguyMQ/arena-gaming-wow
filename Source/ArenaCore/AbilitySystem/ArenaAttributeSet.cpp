// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/ArenaAttributeSet.h"
#include "Characters/ArenaCharacter.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UArenaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, Rage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, MaxRage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, ComboPoints, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, SpellPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, HastePct, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, CritPct, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, DamageTakenMult, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, HealingTakenMult, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenaAttributeSet, Absorb, COND_None, REPNOTIFY_Always);
}

void UArenaAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, FMath::Max(1.f, GetMaxHealth()));
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, FMath::Max(0.f, GetMaxMana()));
	}
	else if (Attribute == GetRageAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, FMath::Max(0.f, GetMaxRage()));
	}
	else if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, FMath::Max(0.f, GetMaxEnergy()));
	}
	else if (Attribute == GetComboPointsAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 5.f);
	}
	else if (Attribute == GetAbsorbAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UArenaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float IncomingDamage = GetDamage();
		SetDamage(0.f);
		if (IncomingDamage <= 0.f)
		{
			return;
		}

		float Remaining = IncomingDamage;
		const float CurrentAbsorb = GetAbsorb();
		if (CurrentAbsorb > 0.f)
		{
			const float Absorbed = FMath::Min(CurrentAbsorb, Remaining);
			SetAbsorb(CurrentAbsorb - Absorbed);
			Remaining -= Absorbed;
		}

		if (Remaining > 0.f)
		{
			SetHealth(FMath::Clamp(GetHealth() - Remaining, 0.f, GetMaxHealth()));
		}

		if (AArenaCharacter* Victim = Cast<AArenaCharacter>(Data.Target.GetAvatarActor()))
		{
			Victim->MulticastCombatFeedback(IncomingDamage, 0);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		const float IncomingHealing = GetHealing();
		SetHealing(0.f);
		if (IncomingHealing <= 0.f)
		{
			return;
		}

		SetHealth(FMath::Clamp(GetHealth() + IncomingHealing, 0.f, GetMaxHealth()));

		if (AArenaCharacter* Target = Cast<AArenaCharacter>(Data.Target.GetAvatarActor()))
		{
			Target->MulticastCombatFeedback(IncomingHealing, 1);
		}
	}
}

#define ARENA_ATTR_REPNOTIFY_IMPL(PropertyName) \
	void UArenaAttributeSet::OnRep_##PropertyName(const FGameplayAttributeData& OldValue) \
	{ \
		GAMEPLAYATTRIBUTE_REPNOTIFY(UArenaAttributeSet, PropertyName, OldValue); \
	}

ARENA_ATTR_REPNOTIFY_IMPL(Health)
ARENA_ATTR_REPNOTIFY_IMPL(MaxHealth)
ARENA_ATTR_REPNOTIFY_IMPL(Mana)
ARENA_ATTR_REPNOTIFY_IMPL(MaxMana)
ARENA_ATTR_REPNOTIFY_IMPL(Rage)
ARENA_ATTR_REPNOTIFY_IMPL(MaxRage)
ARENA_ATTR_REPNOTIFY_IMPL(Energy)
ARENA_ATTR_REPNOTIFY_IMPL(MaxEnergy)
ARENA_ATTR_REPNOTIFY_IMPL(ComboPoints)
ARENA_ATTR_REPNOTIFY_IMPL(AttackPower)
ARENA_ATTR_REPNOTIFY_IMPL(SpellPower)
ARENA_ATTR_REPNOTIFY_IMPL(HastePct)
ARENA_ATTR_REPNOTIFY_IMPL(CritPct)
ARENA_ATTR_REPNOTIFY_IMPL(DamageTakenMult)
ARENA_ATTR_REPNOTIFY_IMPL(HealingTakenMult)
ARENA_ATTR_REPNOTIFY_IMPL(Absorb)

#undef ARENA_ATTR_REPNOTIFY_IMPL
