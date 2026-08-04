// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ArenaDataSubsystem.generated.h"

class UDataTable;

// Charge tous les CSV de Content/Data/ en DataTables au démarrage, sur serveur
// comme sur client. `Arena.ReloadData` (console) recharge tout à chaud pour
// itérer sur l'équilibrage et la géométrie d'arène sans recompiler.
UCLASS()
class ARENACORE_API UArenaDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ReloadAllData();

	const UDataTable* GetTable(FName TableId) const;

	template <typename TRow>
	const TRow* FindRow(FName TableId, FName RowName) const
	{
		const UDataTable* Table = GetTable(TableId);
		return Table ? Table->FindRow<TRow>(RowName, TEXT("ArenaDataSubsystem"), false) : nullptr;
	}

	// Diffusé après chaque rechargement — l'ArenaBuilder s'y abonne pour se reconstruire.
	FSimpleMulticastDelegate OnDataReloaded;

	static const FName TableId_Abilities;
	static const FName TableId_Classes;
	static const FName TableId_Races;
	static const FName TableId_StatTemplates;
	static const FName TableId_ArenaLayout;
	static const FName TableId_Avatars;
	static const FName TableId_Cues;

private:
	void LoadTable(FName TableId, UScriptStruct* RowStruct, const FString& FileName);

	UPROPERTY()
	TMap<FName, TObjectPtr<UDataTable>> Tables;
};
