// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ArenaDataRows.generated.h"

// Structures de lignes des CSV de Content/Data/.
// Règle : uniquement des types simples (FName/FString/float/int32/bool) — les tags
// et enums sont résolus au chargement, jamais importés directement depuis le CSV.
// Les noms de colonnes CSV doivent correspondre EXACTEMENT aux noms de propriétés.

// ArenaLayout.csv — géométrie de l'arène, construite par AArenaBuilder.
// ElementType : Floor | Wall | Pillar | Gate | Spawn ; Shape : Cube | Cylinder.
USTRUCT(BlueprintType)
struct FArenaLayoutRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ElementType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName Shape = FName("Cube");
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float X = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Y = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Z = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Yaw = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ScaleX = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ScaleY = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ScaleZ = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ColorR = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ColorG = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ColorB = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 Team = -1;
};

// Abilities.csv — définition data-driven de tous les sorts (le cœur du jeu).
// MiscValue/MiscValue2 : sémantique propre à chaque sort, documentée dans docs/TUNING.md.
USTRUCT(BlueprintType)
struct FAbilityRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ClassId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString DisplayName;
	// Classe C++ générique qui sert ce sort : InstantDamage, CastedSpell,
	// Interrupt (M3)… Les verbes non implémentés occupent leur slot (label UI)
	// sans être donnés au joueur.
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName Verb = FName("None");
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 Slot = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CastTimeSec = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bOnGCD = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bUsableWhileCC = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CooldownSec = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName School = FName("Physical");
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ResourceType = FName("None");
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Cost = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 ComboGen = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bFinisher = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float DamageBase = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float DamageAPCoef = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float DamageSPCoef = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float HealBase = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float HealSPCoef = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float AbsorbPctMaxHealth = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName CCCategory = FName("None");
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CCDurationSec = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bCCBreaksOnDamage = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MoveSpeedMultPct = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float DebuffDurationSec = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float HealingTakenMultPct = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float DamageTakenMultPct = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float DamageDoneMultPct = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float RangeM = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bRequiresFacing = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bRequiresLoS = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAlliedTarget = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bSelfOnly = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bFromStealthOnly = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAoE = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float PeriodSec = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MiscValue = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MiscValue2 = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString GrantedTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString Description;
};

// Classes.csv — métadonnées des classes jouables.
USTRUCT(BlueprintType)
struct FClassRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ResourceType = FName("Mana");
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ColorR = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ColorG = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ColorB = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString Description;
};

// Races.csv — les deux races et leurs bonus mineurs.
USTRUCT(BlueprintType)
struct FRaceRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float SecondaryStatMultPct = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float StunDurationTakenMultPct = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ActiveAbility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString Description;
};

// StatTemplates.csv — équipement normalisé : les stats de base par classe.
USTRUCT(BlueprintType)
struct FStatTemplateRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxHealth = 20000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxMana = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ManaRegenPerSec = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxRage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxEnergy = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float EnergyRegenPerSec = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float AttackPower = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float SpellPower = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float HastePct = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CritPct = 5.f;
};

// Avatars.csv — chemins soft vers les meshes/AnimBP ; vide = fallback capsule.
USTRUCT(BlueprintType)
struct FAvatarRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString MeshPath;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString AnimBPPath;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Scale = 1.f;
};

// Cues.csv — feedback visuel/sonore minimal par cue (flash, son, shake).
USTRUCT(BlueprintType)
struct FCueRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) float FlashColorR = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float FlashColorG = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float FlashColorB = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString SoundPath;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CameraShakeScale = 0.f;
};
