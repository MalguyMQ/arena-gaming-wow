// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/ArenaAbilitySystemComponent.h"
#include "ArenaGameplayTags.h"
#include "Characters/ArenaCharacter.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

UArenaAbilitySystemComponent::UArenaAbilitySystemComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
}

void UArenaAbilitySystemComponent::TryActivateSlot(int32 Slot)
{
	const float Remaining = GetBlockedRemainingForSlot(Slot);
	if (Remaining <= 0.f)
	{
		QueuedSlot = INDEX_NONE;
		AbilityLocalInputPressed(Slot);
		return;
	}

	if (Remaining <= SpellQueueWindowSec)
	{
		QueuedSlot = Slot;
		QueueExpireTime = GetWorld()->GetTimeSeconds() + Remaining + 0.25;
	}
}

void UArenaAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (QueuedSlot != INDEX_NONE)
	{
		if (GetBlockedRemainingForSlot(QueuedSlot) <= 0.f)
		{
			const int32 Slot = QueuedSlot;
			QueuedSlot = INDEX_NONE;
			AbilityLocalInputPressed(Slot);
		}
		else if (GetWorld()->GetTimeSeconds() > QueueExpireTime)
		{
			QueuedSlot = INDEX_NONE;
		}
	}
}

float UArenaAbilitySystemComponent::GetBlockedRemainingForSlot(int32 Slot) const
{
	float Remaining = 0.f;

	const FGameplayEffectQuery GCDQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(
		FGameplayTagContainer(ArenaTags::Cooldown_GCD));
	for (const float Time : GetActiveEffectsTimeRemaining(GCDQuery))
	{
		Remaining = FMath::Max(Remaining, Time);
	}

	const FGameplayEffectQuery SlotQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(
		FGameplayTagContainer(ArenaTags::CooldownSlotTag(Slot)));
	for (const float Time : GetActiveEffectsTimeRemaining(SlotQuery))
	{
		Remaining = FMath::Max(Remaining, Time);
	}

	// Incantation en cours : on peut mettre le prochain sort en file pendant la fin du cast.
	if (const AArenaCharacter* Character = Cast<AArenaCharacter>(GetAvatarActor()))
	{
		const UWorld* World = GetWorld();
		const float ServerNow = (World && World->GetGameState())
			? World->GetGameState()->GetServerWorldTimeSeconds()
			: (World ? World->GetTimeSeconds() : 0.f);
		if (Character->GetCastState().IsActive(ServerNow))
		{
			Remaining = FMath::Max(Remaining, Character->GetCastState().EndServerTime - ServerNow);
		}
	}

	return Remaining;
}
