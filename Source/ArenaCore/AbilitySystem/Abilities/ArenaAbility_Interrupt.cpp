// Copyright Flagcat Studio. All Rights Reserved.

#include "AbilitySystem/Abilities/ArenaAbility_Interrupt.h"
#include "AbilitySystem/Effects/ArenaGameplayEffects.h"
#include "ArenaGameplayTags.h"
#include "ArenaCore.h"
#include "Characters/ArenaCharacter.h"
#include "System/ArenaDataSubsystem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameStateBase.h"

void UArenaAbility_Interrupt::ActivateAbility(
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
		AArenaCharacter* TargetCharacter = Cast<AArenaCharacter>(Target);
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
		const UWorld* World = GetWorld();
		const float ServerNow = (World && World->GetGameState())
			? World->GetGameState()->GetServerWorldTimeSeconds()
			: (World ? World->GetTimeSeconds() : 0.f);

		if (TargetCharacter && TargetASC && TargetCharacter->GetCastState().IsActive(ServerNow))
		{
			const FName InterruptedRowName = TargetCharacter->GetCastState().AbilityRow;

			const UGameInstance* GI = TargetCharacter->GetGameInstance();
			UArenaDataSubsystem* Data = GI ? GI->GetSubsystem<UArenaDataSubsystem>() : nullptr;
			const FAbilityRow* InterruptedRow = Data
				? Data->FindRow<FAbilityRow>(UArenaDataSubsystem::TableId_Abilities, InterruptedRowName)
				: nullptr;

			TargetASC->CancelAbilities();
			TargetCharacter->ClearCastState();

			const float LockoutDuration = Row->MiscValue > 0.f ? Row->MiscValue : 4.f;
			const FName InterruptedSchool = InterruptedRow ? InterruptedRow->School : FName("Physical");

			FGameplayEffectSpecHandle LockoutSpec = MakeOutgoingGameplayEffectSpec(
				Handle, ActorInfo, ActivationInfo, UArenaGE_SchoolLockout::StaticClass(), 1.f);
			if (LockoutSpec.IsValid())
			{
				LockoutSpec.Data->SetSetByCallerMagnitude(ArenaTags::Data_Duration, LockoutDuration);
				LockoutSpec.Data->DynamicGrantedTags.AddTag(ArenaTags::LockoutTagFromName(InterruptedSchool));
				if (UAbilitySystemComponent* SourceASC = ActorInfo->AbilitySystemComponent.Get())
				{
					SourceASC->ApplyGameplayEffectSpecToTarget(*LockoutSpec.Data.Get(), TargetASC);
				}
			}

			TargetCharacter->MulticastCombatFeedback(0.f, 2);
			UE_LOG(LogArena, Log, TEXT("Interruption : %s verrouillé (école %s) %.0f s"),
				*InterruptedRowName.ToString(), *InterruptedSchool.ToString(), LockoutDuration);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
