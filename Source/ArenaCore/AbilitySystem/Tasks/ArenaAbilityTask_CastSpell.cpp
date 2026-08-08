// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/Tasks/ArenaAbilityTask_CastSpell.h"
#include "GameFramework/Actor.h"

UArenaAbilityTask_CastSpell* UArenaAbilityTask_CastSpell::CastSpell(UGameplayAbility* OwningAbility, float InDuration, bool bInCancelOnMove)
{
	UArenaAbilityTask_CastSpell* Task = NewAbilityTask<UArenaAbilityTask_CastSpell>(OwningAbility);
	Task->Duration = FMath::Max(0.05f, InDuration);
	Task->bCancelOnMove = bInCancelOnMove;
	Task->bTickingTask = true;
	return Task;
}

void UArenaAbilityTask_CastSpell::Activate()
{
	Super::Activate();
}

void UArenaAbilityTask_CastSpell::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (bDone)
	{
		return;
	}

	if (bCancelOnMove)
	{
		const AActor* Avatar = GetAvatarActor();
		if (Avatar && Avatar->GetVelocity().SizeSquared2D() > MoveCancelSpeed * MoveCancelSpeed)
		{
			bDone = true;
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnCastCancelled.Broadcast();
			}
			EndTask();
			return;
		}
	}

	Elapsed += DeltaTime;
	if (Elapsed >= Duration)
	{
		bDone = true;
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCastFinished.Broadcast();
		}
		EndTask();
	}
}
