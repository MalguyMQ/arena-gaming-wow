// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/ArenaGameplayAbility.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "AbilitySystem/Effects/ArenaGameplayEffects.h"
#include "ArenaGameplayTags.h"
#include "ArenaCore.h"
#include "Characters/ArenaCharacter.h"
#include "Combat/ArenaTargetingComponent.h"
#include "System/ArenaDataSubsystem.h"
#include "AbilitySystemComponent.h"
#include "Engine/GameInstance.h"

UArenaGameplayAbility::UArenaGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UArenaGameplayAbility::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const FAbilityRow* Row = GetAbilityRow(Handle, ActorInfo);
	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!Row || !ASC)
	{
		return false;
	}

	if (Row->bOnGCD && ASC->HasMatchingGameplayTag(ArenaTags::Cooldown_GCD))
	{
		return false;
	}
	if (Row->CooldownSec > 0.f && ASC->HasMatchingGameplayTag(ArenaTags::CooldownSlotTag(Row->Slot)))
	{
		return false;
	}

	return true;
}

void UArenaGameplayAbility::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const FAbilityRow* Row = GetAbilityRow(Handle, ActorInfo);
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!Row || !ASC)
	{
		return;
	}

	if (Row->bOnGCD)
	{
		const float Haste = FMath::Max(0.f, ASC->GetNumericAttribute(UArenaAttributeSet::GetHastePctAttribute())) / 100.f;
		const float GCDDuration = FMath::Clamp(GCDBaseSeconds / (1.f + Haste), GCDFloorSeconds, GCDBaseSeconds);

		FGameplayEffectSpecHandle GCDSpec = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, UArenaGE_GlobalCooldown::StaticClass(), 1.f);
		if (GCDSpec.IsValid())
		{
			GCDSpec.Data->SetSetByCallerMagnitude(ArenaTags::Data_Duration, GCDDuration);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, GCDSpec);
		}
	}

	if (Row->CooldownSec > 0.f)
	{
		FGameplayEffectSpecHandle CooldownSpec = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, UArenaGE_AbilityCooldown::StaticClass(), 1.f);
		if (CooldownSpec.IsValid())
		{
			CooldownSpec.Data->SetSetByCallerMagnitude(ArenaTags::Data_Duration, Row->CooldownSec);
			CooldownSpec.Data->DynamicGrantedTags.AddTag(ArenaTags::CooldownSlotTag(Row->Slot));
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CooldownSpec);
		}
	}
}

const FAbilityRow* UArenaGameplayAbility::GetAbilityRow(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo)
	{
		return nullptr;
	}

	const AArenaCharacter* Character = Cast<AArenaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!Character || !ASC)
	{
		return nullptr;
	}

	const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle);
	if (!Spec)
	{
		return nullptr;
	}

	const FName RowName = Character->GetAbilityRowForSlot(Spec->InputID);
	if (RowName.IsNone())
	{
		return nullptr;
	}

	const UGameInstance* GI = Character->GetGameInstance();
	UArenaDataSubsystem* Data = GI ? GI->GetSubsystem<UArenaDataSubsystem>() : nullptr;
	return Data ? Data->FindRow<FAbilityRow>(UArenaDataSubsystem::TableId_Abilities, RowName) : nullptr;
}

AActor* UArenaGameplayAbility::GetValidatedTarget(
	const FAbilityRow& Row,
	const FGameplayAbilityActorInfo* ActorInfo) const
{
	const AArenaCharacter* Character = Cast<AArenaCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character || !Character->GetTargetingComponent())
	{
		return nullptr;
	}

	AActor* Target = Character->GetTargetingComponent()->GetCurrentTarget();
	if (!Target || Target == Character)
	{
		return nullptr;
	}

	if (Row.RangeM > 0.f)
	{
		const float MaxDistCm = Row.RangeM * 100.f + RangeSlackCm;
		if (FVector::Dist(Character->GetActorLocation(), Target->GetActorLocation()) > MaxDistCm)
		{
			return nullptr;
		}
	}

	return Target;
}
