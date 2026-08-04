// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ArenaInputSetup.generated.h"

class UInputAction;
class UInputMappingContext;
class APlayerController;

// Construit tout l'Enhanced Input au runtime (aucun asset d'input) :
// les touches viennent de Config/DefaultArenaKeybinds.ini — rebinder = éditer l'ini.
// Outer = PlayerController local, pour que le GC ne récupère pas les actions.
UCLASS()
class ARENACORE_API UArenaInputSetup : public UObject
{
	GENERATED_BODY()

public:
	void Build(APlayerController* PlayerController);

	UPROPERTY() TObjectPtr<UInputAction> IA_MoveForward;
	UPROPERTY() TObjectPtr<UInputAction> IA_Strafe;
	UPROPERTY() TObjectPtr<UInputAction> IA_Turn;
	UPROPERTY() TObjectPtr<UInputAction> IA_Jump;
	UPROPERTY() TObjectPtr<UInputAction> IA_Look;
	UPROPERTY() TObjectPtr<UInputAction> IA_Zoom;
	UPROPERTY() TObjectPtr<UInputMappingContext> IMC_Default;

	// Réglages caméra lus depuis l'ini ([Camera])
	float MouseSensitivity = 1.f;
	bool bInvertY = false;
	float ZoomStep = 50.f;
};
