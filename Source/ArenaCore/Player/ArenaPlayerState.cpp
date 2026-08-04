// Copyright Flagcat Studio. All Rights Reserved.

#include "Player/ArenaPlayerState.h"
#include "Net/UnrealNetwork.h"

void AArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArenaPlayerState, TeamIndex);
	DOREPLIFETIME(AArenaPlayerState, ClassId);
}
