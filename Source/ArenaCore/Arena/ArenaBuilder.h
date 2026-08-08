// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/ArenaDataRows.h"
#include "ArenaBuilder.generated.h"

class UStaticMeshComponent;
class UDirectionalLightComponent;
class UStaticMesh;
class UMaterialInterface;

// Construit toute la géométrie de l'arène (sol, murs, piliers, portes) depuis
// ArenaLayout.csv avec les meshes de base du moteur — la map .umap reste vide.
// Répliqué : chaque machine (serveur et clients) reconstruit localement la même
// géométrie de façon déterministe depuis ses CSV packagés. Zéro asset, zéro bande passante.
// L'éclairage (key + fill) fait partie de l'acteur.
UCLASS()
class ARENACORE_API AArenaBuilder : public AActor
{
	GENERATED_BODY()

public:
	AArenaBuilder();

	void Rebuild();

	// Détruit les portes de départ (préparation terminée). Provisoire : timer
	// local sur chaque machine — M5 le pilotera depuis la phase de match répliquée.
	void OpenGates();

	// Secondes avant l'ouverture des portes ; <= 0 si déjà ouvertes.
	float GetGateTimeRemaining() const;

	UPROPERTY(EditAnywhere, Category = "Arena")
	float GateOpenDelaySec = 15.f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void SpawnElement(const FName& RowName, const FArenaLayoutRow& Row);
	void ClearBuilt();

	FTimerHandle GateTimerHandle;

	UPROPERTY() TObjectPtr<USceneComponent> Root;
	UPROPERTY() TObjectPtr<UDirectionalLightComponent> KeyLight;
	UPROPERTY() TObjectPtr<UDirectionalLightComponent> FillLight;
	UPROPERTY() TObjectPtr<UStaticMesh> CubeMesh;
	UPROPERTY() TObjectPtr<UStaticMesh> CylinderMesh;
	UPROPERTY() TObjectPtr<UMaterialInterface> BaseMaterial;
	UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> BuiltComponents;

	FDelegateHandle ReloadHandle;
};
