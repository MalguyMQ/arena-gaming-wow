// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ArenaAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// Set d'attributs unique pour toutes les classes : les attributs inutilisés
// restent à zéro sans coût. Valeurs de base injectées depuis StatTemplates.csv.
// `Damage` et `Healing` sont des méta-attributs (non répliqués) : réceptacles
// de sortie des ExecCalc, consommés dans PostGameplayEffectExecute.
UCLASS()
class ARENACORE_API UArenaAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, MaxMana)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Rage)
	FGameplayAttributeData Rage;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, Rage)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxRage)
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, MaxRage)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, Energy)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, MaxEnergy)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ComboPoints)
	FGameplayAttributeData ComboPoints;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, ComboPoints)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpellPower)
	FGameplayAttributeData SpellPower;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, SpellPower)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HastePct)
	FGameplayAttributeData HastePct;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, HastePct)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritPct)
	FGameplayAttributeData CritPct;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, CritPct)

	// Multiplicateurs (1.0 = neutre) : Frappe brutale, Suppression de la douleur…
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageTakenMult)
	FGameplayAttributeData DamageTakenMult;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, DamageTakenMult)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealingTakenMult)
	FGameplayAttributeData HealingTakenMult;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, HealingTakenMult)

	// Points d'absorption restants (boucliers) — consommés avant les PV.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Absorb)
	FGameplayAttributeData Absorb;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, Absorb)

	// Méta-attributs (non répliqués)
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, Damage)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UArenaAttributeSet, Healing)

protected:
	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Rage(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxRage(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Energy(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxEnergy(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_ComboPoints(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_AttackPower(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_SpellPower(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_HastePct(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CritPct(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_DamageTakenMult(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_HealingTakenMult(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Absorb(const FGameplayAttributeData& OldValue);
};
