// Copyright Flagcat Studio. All Rights Reserved.

#include "ArenaGameplayTags.h"

namespace ArenaTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_GCD, "Cooldown.GCD", "Global cooldown actif");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Ability, "Cooldown.Ability", "Parent des cooldowns par sort");

	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot0, "Cooldown.Ability.Slot0");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot1, "Cooldown.Ability.Slot1");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot2, "Cooldown.Ability.Slot2");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot3, "Cooldown.Ability.Slot3");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot4, "Cooldown.Ability.Slot4");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot5, "Cooldown.Ability.Slot5");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot6, "Cooldown.Ability.Slot6");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot7, "Cooldown.Ability.Slot7");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot8, "Cooldown.Ability.Slot8");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot9, "Cooldown.Ability.Slot9");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot10, "Cooldown.Ability.Slot10");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Slot11, "Cooldown.Ability.Slot11");

	const FGameplayTag& CooldownSlotTag(int32 Slot)
	{
		static const FNativeGameplayTag* SlotTags[] =
		{
			&Cooldown_Slot0, &Cooldown_Slot1, &Cooldown_Slot2, &Cooldown_Slot3,
			&Cooldown_Slot4, &Cooldown_Slot5, &Cooldown_Slot6, &Cooldown_Slot7,
			&Cooldown_Slot8, &Cooldown_Slot9, &Cooldown_Slot10, &Cooldown_Slot11
		};
		const int32 Clamped = FMath::Clamp(Slot, 0, UE_ARRAY_COUNT(SlotTags) - 1);
		return SlotTags[Clamped]->GetTag();
	}

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Casting, "State.Casting", "Une incantation est en cours");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_InCombat, "State.InCombat", "En combat (bloque camouflage, régen…)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Stealth, "State.Stealth", "Camouflé");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "Mort — fin de manche");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CC_Stun, "State.CC.Stun", "Étourdi : aucune action, aucun déplacement");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CC_Incapacitate, "State.CC.Incapacitate", "Incapacité (mouton) : casse aux dégâts");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CC_Root, "State.CC.Root", "Raciné : immobile, actions possibles");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CC_Disorient, "State.CC.Disorient", "Désorienté (peur) : casse aux dégâts");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CC_Silence, "State.CC.Silence", "Silence : sorts magiques bloqués");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CC_Snare, "State.CC.Snare", "Ralenti");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CC_Breakable, "State.CC.Breakable", "CC cassable par le trinket PvP");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CC_BreaksOnDamage, "State.CC.BreaksOnDamage", "CC qui casse quand la cible subit des dégâts");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DR_Stun, "DR.Stun", "Catégorie DR : étourdissements");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DR_Incapacitate, "DR.Incapacitate", "Catégorie DR : incapacités");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DR_Root, "DR.Root", "Catégorie DR : racines");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DR_Disorient, "DR.Disorient", "Catégorie DR : désorientations");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DR_Silence, "DR.Silence", "Catégorie DR : silences");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(School_Physical, "School.Physical", "École physique");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(School_Frost, "School.Frost", "École de givre");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(School_Fire, "School.Fire", "École de feu");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(School_Arcane, "School.Arcane", "École des arcanes");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(School_Holy, "School.Holy", "École sacrée");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(School_Shadow, "School.Shadow", "École d'ombre");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lockout_Physical, "Lockout.School.Physical", "École physique verrouillée");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lockout_Frost, "Lockout.School.Frost", "École de givre verrouillée");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lockout_Fire, "Lockout.School.Fire", "École de feu verrouillée");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lockout_Arcane, "Lockout.School.Arcane", "École des arcanes verrouillée");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lockout_Holy, "Lockout.School.Holy", "École sacrée verrouillée");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lockout_Shadow, "Lockout.School.Shadow", "École d'ombre verrouillée");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Damage, "Data.Damage", "SetByCaller : dégâts de base");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Heal, "Data.Heal", "SetByCaller : soins de base");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Duration, "Data.Duration", "SetByCaller : durée d'effet");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Cost, "Data.Cost", "SetByCaller : coût en ressource");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cue_Damage, "GameplayCue.Arena.Damage", "Feedback de dégâts (combat text, flash)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cue_Heal, "GameplayCue.Arena.Heal", "Feedback de soins");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cue_Immune, "GameplayCue.Arena.Immune", "Feedback d'immunité (DR niveau 4)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cue_Interrupt, "GameplayCue.Arena.Interrupt", "Feedback d'interruption");
}
