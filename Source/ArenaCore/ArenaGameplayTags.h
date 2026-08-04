// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

// Tags natifs du jeu — source de vérité unique, vérifiés à la compilation.
// Aucun tag défini en ini : tout nouveau tag s'ajoute ici et dans le .cpp.
namespace ArenaTags
{
	// Cooldowns
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_GCD);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Ability);

	// États généraux
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Casting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_InCombat);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stealth);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);

	// Contrôles (CC)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CC_Stun);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CC_Incapacitate);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CC_Root);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CC_Disorient);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CC_Silence);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CC_Snare);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CC_Breakable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CC_BreaksOnDamage);

	// Catégories de rendements décroissants (DR)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DR_Stun);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DR_Incapacitate);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DR_Root);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DR_Disorient);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DR_Silence);

	// Écoles de sorts
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(School_Physical);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(School_Frost);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(School_Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(School_Arcane);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(School_Holy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(School_Shadow);

	// Verrouillages d'école (après interruption)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lockout_Physical);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lockout_Frost);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lockout_Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lockout_Arcane);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lockout_Holy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lockout_Shadow);

	// SetByCaller (magnitudes injectées depuis les CSV)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Heal);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Duration);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Cost);

	// GameplayCues
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cue_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cue_Heal);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cue_Immune);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cue_Interrupt);
}
