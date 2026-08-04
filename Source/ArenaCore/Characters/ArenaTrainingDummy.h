// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ArenaCharacter.h"
#include "ArenaTrainingDummy.generated.h"

// Mannequin d'entraînement : un ArenaCharacter sans contrôleur, immobile,
// qui régénère tous ses PV 5 s après le dernier dégât reçu.
// Positions définies dans ArenaLayout.csv (ElementType=Dummy), spawn par le GameMode.
UCLASS()
class ARENACORE_API AArenaTrainingDummy : public AArenaCharacter
{
	GENERATED_BODY()

public:
	AArenaTrainingDummy(const FObjectInitializer& ObjectInitializer);

	virtual FName GetClassId() const override;
	virtual FText GetDisplayName() const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Arena")
	float HealthResetDelaySec = 5.f;

private:
	void CheckHealthReset();

	float LastDamageTime = -1.f;
	FTimerHandle ResetTimerHandle;
};
