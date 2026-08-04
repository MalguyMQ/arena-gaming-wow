// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArenaPlayerController.generated.h"

class UArenaInputSetup;

// Contrôleur joueur : curseur libre hors clic (mode WoW), capture souris pendant
// les clics maintenus (voir DefaultViewportMouseCaptureMode dans DefaultInput.ini).
UCLASS()
class ARENACORE_API AArenaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Construit (paresseusement) l'input runtime pour ce joueur local.
	UArenaInputSetup* GetInputSetup();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UArenaInputSetup> InputSetup;
};
