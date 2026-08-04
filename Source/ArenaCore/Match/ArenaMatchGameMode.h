// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaMatchGameMode.generated.h"

// GameMode du duel : assigne les équipes, place les joueurs à leurs portes,
// et fait construire l'arène procédurale. Défini comme GlobalDefaultGameMode
// dans DefaultEngine.ini — la map reste un niveau vide.
UCLASS()
class ARENACORE_API AArenaMatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaMatchGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	void SpawnPlayerStartsFromLayout();
};
