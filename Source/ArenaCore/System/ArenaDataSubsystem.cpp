// Copyright Flagcat Studio. All Rights Reserved.

#include "System/ArenaDataSubsystem.h"
#include "System/ArenaDataRows.h"
#include "ArenaCore.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

const FName UArenaDataSubsystem::TableId_Abilities(TEXT("Abilities"));
const FName UArenaDataSubsystem::TableId_Classes(TEXT("Classes"));
const FName UArenaDataSubsystem::TableId_Races(TEXT("Races"));
const FName UArenaDataSubsystem::TableId_StatTemplates(TEXT("StatTemplates"));
const FName UArenaDataSubsystem::TableId_ArenaLayout(TEXT("ArenaLayout"));
const FName UArenaDataSubsystem::TableId_Avatars(TEXT("Avatars"));
const FName UArenaDataSubsystem::TableId_Cues(TEXT("Cues"));

static FAutoConsoleCommandWithWorldAndArgs GArenaReloadDataCmd(
	TEXT("Arena.ReloadData"),
	TEXT("Recharge tous les CSV de Content/Data/ et reconstruit l'arène."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
	{
		if (World && World->GetGameInstance())
		{
			if (UArenaDataSubsystem* Data = World->GetGameInstance()->GetSubsystem<UArenaDataSubsystem>())
			{
				Data->ReloadAllData();
			}
		}
	}));

void UArenaDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReloadAllData();
}

void UArenaDataSubsystem::ReloadAllData()
{
	LoadTable(TableId_Abilities, FAbilityRow::StaticStruct(), TEXT("Abilities.csv"));
	LoadTable(TableId_Classes, FClassRow::StaticStruct(), TEXT("Classes.csv"));
	LoadTable(TableId_Races, FRaceRow::StaticStruct(), TEXT("Races.csv"));
	LoadTable(TableId_StatTemplates, FStatTemplateRow::StaticStruct(), TEXT("StatTemplates.csv"));
	LoadTable(TableId_ArenaLayout, FArenaLayoutRow::StaticStruct(), TEXT("ArenaLayout.csv"));
	LoadTable(TableId_Avatars, FAvatarRow::StaticStruct(), TEXT("Avatars.csv"));
	LoadTable(TableId_Cues, FCueRow::StaticStruct(), TEXT("Cues.csv"));

	OnDataReloaded.Broadcast();
}

const UDataTable* UArenaDataSubsystem::GetTable(FName TableId) const
{
	const TObjectPtr<UDataTable>* Found = Tables.Find(TableId);
	return Found ? Found->Get() : nullptr;
}

void UArenaDataSubsystem::LoadTable(FName TableId, UScriptStruct* RowStruct, const FString& FileName)
{
	const FString FilePath = FPaths::ProjectContentDir() / TEXT("Data") / FileName;

	FString CsvContent;
	if (!FFileHelper::LoadFileToString(CsvContent, *FilePath))
	{
		UE_LOG(LogArena, Error, TEXT("Table '%s' : fichier introuvable ou illisible : %s"), *TableId.ToString(), *FilePath);
		return;
	}

	UDataTable* Table = NewObject<UDataTable>(this);
	Table->RowStruct = RowStruct;

	const TArray<FString> Problems = Table->CreateTableFromCSVString(CsvContent);
	for (const FString& Problem : Problems)
	{
		UE_LOG(LogArena, Error, TEXT("Table '%s' : %s"), *TableId.ToString(), *Problem);
	}

	Tables.Add(TableId, Table);
	UE_LOG(LogArena, Log, TEXT("Table '%s' chargée : %d lignes (%s)"),
		*TableId.ToString(), Table->GetRowMap().Num(), *FileName);
}
