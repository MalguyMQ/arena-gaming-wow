// Copyright Flagcat Studio. All Rights Reserved.

#include "Match/ArenaMatchGameMode.h"
#include "Match/ArenaGameState.h"
#include "Characters/ArenaCharacter.h"
#include "Characters/ArenaTrainingDummy.h"
#include "Player/ArenaPlayerController.h"
#include "Player/ArenaPlayerState.h"
#include "UI/ArenaHUD.h"
#include "Arena/ArenaBuilder.h"
#include "System/ArenaDataSubsystem.h"
#include "System/ArenaDataRows.h"
#include "ArenaCore.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

AArenaMatchGameMode::AArenaMatchGameMode()
{
	DefaultPawnClass = AArenaCharacter::StaticClass();
	PlayerControllerClass = AArenaPlayerController::StaticClass();
	PlayerStateClass = AArenaPlayerState::StaticClass();
	GameStateClass = AArenaGameState::StaticClass();
	HUDClass = AArenaHUD::StaticClass();
}

void AArenaMatchGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	if (UWorld* World = GetWorld())
	{
		World->SpawnActor<AArenaBuilder>(FVector::ZeroVector, FRotator::ZeroRotator);
	}
	SpawnPlayerStartsFromLayout();
}

void AArenaMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (AArenaPlayerState* PS = NewPlayer ? NewPlayer->GetPlayerState<AArenaPlayerState>() : nullptr)
	{
		PS->TeamIndex = (FMath::Max(1, GetNumPlayers()) - 1) % 2;
	}
	Super::PostLogin(NewPlayer);
}

AActor* AArenaMatchGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	const AArenaPlayerState* PS = Player ? Player->GetPlayerState<AArenaPlayerState>() : nullptr;
	const FName WantedTag(*FString::Printf(TEXT("Team%d"), PS ? PS->TeamIndex : 0));

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (It->PlayerStartTag == WantedTag)
		{
			return *It;
		}
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AArenaMatchGameMode::SpawnPlayerStartsFromLayout()
{
	UGameInstance* GI = GetGameInstance();
	UArenaDataSubsystem* Data = GI ? GI->GetSubsystem<UArenaDataSubsystem>() : nullptr;
	const UDataTable* Layout = Data ? Data->GetTable(UArenaDataSubsystem::TableId_ArenaLayout) : nullptr;
	if (!Layout)
	{
		UE_LOG(LogArena, Error, TEXT("ArenaLayout introuvable : aucun point de spawn créé."));
		return;
	}

	Layout->ForeachRow<FArenaLayoutRow>(TEXT("SpawnPoints"), [this](const FName& RowName, const FArenaLayoutRow& Row)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (Row.ElementType == FName("Spawn"))
		{
			APlayerStart* Start = GetWorld()->SpawnActor<APlayerStart>(
				FVector(Row.X, Row.Y, Row.Z), FRotator(0.f, Row.Yaw, 0.f), Params);
			if (Start)
			{
				Start->PlayerStartTag = FName(*FString::Printf(TEXT("Team%d"), FMath::Max(0, Row.Team)));
			}
		}
		else if (Row.ElementType == FName("Dummy"))
		{
			GetWorld()->SpawnActor<AArenaTrainingDummy>(
				FVector(Row.X, Row.Y, Row.Z), FRotator(0.f, Row.Yaw, 0.f), Params);
		}
	});
}
