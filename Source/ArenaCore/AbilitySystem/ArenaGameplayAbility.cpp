// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/ArenaGameplayAbility.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "AbilitySystem/Effects/ArenaGameplayEffects.h"
#include "ArenaGameplayTags.h"
#include "ArenaCore.h"
#include "Characters/ArenaCharacter.h"
#include "Combat/ArenaTargetingComponent.h"
#include "Player/ArenaPlayerState.h"
#include "System/ArenaDataSubsystem.h"
#include "AbilitySystemComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

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

	// Verrouillage d'école (interruption subie) — ne concerne pas le physique.
	if (Row->School != FName("Physical") && ASC->HasMatchingGameplayTag(ArenaTags::LockoutTagFromName(Row->School)))
	{
		return false;
	}

	// Pendant sa propre incantation, seule une interruption reste utilisable.
	if (ASC->HasMatchingGameplayTag(ArenaTags::State_Casting) && Row->Verb != FName("Interrupt"))
	{
		return false;
	}

	return true;
}

bool UArenaGameplayAbility::CheckCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
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

bool UArenaGameplayAbility::CheckCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	const FAbilityRow* Row = GetAbilityRow(Handle, ActorInfo);
	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!Row || !ASC)
	{
		return false;
	}

	if (Row->Cost <= 0.f || Row->ResourceType == FName("None"))
	{
		return true;
	}
	return GetResourceCurrent(ASC, Row->ResourceType) >= Row->Cost;
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

void UArenaGameplayAbility::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const FAbilityRow* Row = GetAbilityRow(Handle, ActorInfo);
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!Row || !ASC || Row->ResourceType == FName("None") || FMath::IsNearlyZero(Row->Cost))
	{
		return;
	}

	// Serveur uniquement : les attributs répliquent, pas de double débit prédictif.
	if (ActorInfo->IsNetAuthority())
	{
		const FGameplayAttribute Attribute = ResourceAttributeFromName(Row->ResourceType);
		if (Attribute.IsValid())
		{
			ASC->ApplyModToAttribute(Attribute, EGameplayModOp::Additive, -Row->Cost);
		}
	}
}

bool UArenaGameplayAbility::AreAllies(const AArenaCharacter* A, const AArenaCharacter* B)
{
	if (!A || !B)
	{
		return false;
	}
	const AArenaPlayerState* PSA = A->GetPlayerState<AArenaPlayerState>();
	const AArenaPlayerState* PSB = B->GetPlayerState<AArenaPlayerState>();
	return PSA && PSB && PSA->TeamIndex == PSB->TeamIndex;
}

bool UArenaGameplayAbility::HasLineOfSight(const AActor* From, const AActor* To)
{
	if (!From || !To)
	{
		return false;
	}
	UWorld* World = From->GetWorld();
	if (!World)
	{
		return false;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ArenaLoS), /*bTraceComplex*/ false);
	Params.AddIgnoredActor(From);
	Params.AddIgnoredActor(To);

	const FVector Start = From->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const FVector End = To->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	return !World->LineTraceTestByChannel(Start, End, ECC_Visibility, Params);
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
	const FGameplayAbilityActorInfo* ActorInfo,
	bool bCheckLoS) const
{
	AArenaCharacter* Character = Cast<AArenaCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		return nullptr;
	}

	if (Row.bSelfOnly)
	{
		return Character;
	}

	AActor* Target = Character->GetTargetingComponent()
		? Character->GetTargetingComponent()->GetCurrentTarget()
		: nullptr;

	if (Row.bAlliedTarget)
	{
		// Sort allié : cible courante si alliée, sinon soi-même (réflexe WoW).
		AArenaCharacter* TargetCharacter = Cast<AArenaCharacter>(Target);
		if (!TargetCharacter || (TargetCharacter != Character && !AreAllies(Character, TargetCharacter)))
		{
			Target = Character;
		}
		if (Target == Character)
		{
			return Target;
		}
	}
	else if (!Target || Target == Character)
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

	if (Row.bRequiresFacing)
	{
		const FVector ToTarget = (Target->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal2D();
		if (FVector::DotProduct(Character->GetActorForwardVector().GetSafeNormal2D(), ToTarget) <= 0.f)
		{
			return nullptr;
		}
	}

	if (Row.bRequiresLoS && bCheckLoS && !HasLineOfSight(Character, Target))
	{
		return nullptr;
	}

	return Target;
}

FGameplayAttribute UArenaGameplayAbility::ResourceAttributeFromName(FName ResourceType)
{
	if (ResourceType == FName("Mana"))
	{
		return UArenaAttributeSet::GetManaAttribute();
	}
	if (ResourceType == FName("Rage"))
	{
		return UArenaAttributeSet::GetRageAttribute();
	}
	if (ResourceType == FName("Energy"))
	{
		return UArenaAttributeSet::GetEnergyAttribute();
	}
	return FGameplayAttribute();
}

float UArenaGameplayAbility::GetResourceCurrent(const UAbilitySystemComponent* ASC, FName ResourceType)
{
	const FGameplayAttribute Attribute = ResourceAttributeFromName(ResourceType);
	return (ASC && Attribute.IsValid()) ? ASC->GetNumericAttribute(Attribute) : 0.f;
}
