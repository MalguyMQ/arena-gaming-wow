// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ArenaCharacterMovementComponent.generated.h"

// Mouvement à la WoW : reculade ralentie, arrêts/départs secs.
// M4 : la vitesse passera à 0 sous les tags State.CC.Root / State.CC.Stun.
UCLASS()
class ARENACORE_API UArenaCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UArenaCharacterMovementComponent();

	virtual float GetMaxSpeed() const override;

	UPROPERTY(EditAnywhere, Category = "Arena")
	float BackpedalSpeedMultiplier = 0.45f;
};
