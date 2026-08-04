// Copyright Flagcat Studio. All Rights Reserved.

#include "Player/ArenaPlayerController.h"
#include "Player/ArenaInputSetup.h"

UArenaInputSetup* AArenaPlayerController::GetInputSetup()
{
	if (!InputSetup && IsLocalController())
	{
		InputSetup = NewObject<UArenaInputSetup>(this);
		InputSetup->Build(this);
	}
	return InputSetup;
}

void AArenaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(true);
		SetInputMode(InputMode);
	}
}
