// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArenaTargetingComponent.generated.h"

// Ciblage tab à la WoW : Tab cycle les personnages proches (tri par distance),
// la cible est posée localement pour la réactivité puis validée côté serveur —
// c'est la cible répliquée qui fait foi pour les sorts.
UCLASS(ClassGroup = (Arena), meta = (BlueprintSpawnableComponent))
class ARENACORE_API UArenaTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UArenaTargetingComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CycleTarget();
	void ClearTarget();

	AActor* GetCurrentTarget() const { return CurrentTarget; }

	FSimpleMulticastDelegate OnTargetChanged;

	UPROPERTY(EditAnywhere, Category = "Arena")
	float MaxTargetingRangeCm = 6000.f;

protected:
	UFUNCTION(Server, Reliable)
	void ServerSetTarget(AActor* NewTarget);

	UFUNCTION()
	void OnRep_CurrentTarget();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTarget)
	TObjectPtr<AActor> CurrentTarget;

private:
	void SetTargetLocal(AActor* NewTarget);
};
