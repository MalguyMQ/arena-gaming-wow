// Copyright Flagcat Studio. All Rights Reserved.

#include "Combat/ArenaTargetingComponent.h"
#include "Characters/ArenaCharacter.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

UArenaTargetingComponent::UArenaTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UArenaTargetingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UArenaTargetingComponent, CurrentTarget);
}

void UArenaTargetingComponent::CycleTarget()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	TArray<AActor*> Candidates;
	for (TActorIterator<AArenaCharacter> It(GetWorld()); It; ++It)
	{
		AArenaCharacter* Candidate = *It;
		if (Candidate == OwnerActor)
		{
			continue;
		}
		if (FVector::Dist(OwnerActor->GetActorLocation(), Candidate->GetActorLocation()) > MaxTargetingRangeCm)
		{
			continue;
		}
		Candidates.Add(Candidate);
	}

	if (Candidates.Num() == 0)
	{
		return;
	}

	Candidates.Sort([OwnerActor](const AActor& A, const AActor& B)
	{
		const FVector Origin = OwnerActor->GetActorLocation();
		return FVector::DistSquared(Origin, A.GetActorLocation()) < FVector::DistSquared(Origin, B.GetActorLocation());
	});

	const int32 CurrentIndex = Candidates.IndexOfByKey(CurrentTarget.Get());
	AActor* NewTarget = Candidates[(CurrentIndex + 1) % Candidates.Num()];
	SetTargetLocal(NewTarget);
}

void UArenaTargetingComponent::ClearTarget()
{
	SetTargetLocal(nullptr);
}

void UArenaTargetingComponent::SetTargetLocal(AActor* NewTarget)
{
	CurrentTarget = NewTarget;
	OnTargetChanged.Broadcast();

	if (AActor* OwnerActor = GetOwner(); OwnerActor && !OwnerActor->HasAuthority())
	{
		ServerSetTarget(NewTarget);
	}
}

void UArenaTargetingComponent::ServerSetTarget_Implementation(AActor* NewTarget)
{
	CurrentTarget = NewTarget;
	OnTargetChanged.Broadcast();
}

void UArenaTargetingComponent::OnRep_CurrentTarget()
{
	OnTargetChanged.Broadcast();
}
