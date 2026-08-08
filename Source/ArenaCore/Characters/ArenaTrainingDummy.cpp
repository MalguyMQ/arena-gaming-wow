// Copyright Flagcat Studio. All Rights Reserved.

#include "Characters/ArenaTrainingDummy.h"
#include "AbilitySystem/ArenaAbilitySystemComponent.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

AArenaTrainingDummy::AArenaTrainingDummy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (GetArenaAbilitySystemComponent())
	{
		GetArenaAbilitySystemComponent()->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	}
	AutoPossessAI = EAutoPossessAI::Disabled;
}

FName AArenaTrainingDummy::GetClassId() const
{
	return FName("Dummy");
}

FText AArenaTrainingDummy::GetDisplayName() const
{
	return NSLOCTEXT("Arena", "TrainingDummy", "Mannequin");
}

void AArenaTrainingDummy::BeginPlay()
{
	Super::BeginPlay();

	if (BodyMesh)
	{
		if (UMaterialInstanceDynamic* MID = BodyMesh->CreateAndSetMaterialInstanceDynamic(0))
		{
			MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.65f, 0.2f, 0.15f));
		}
	}

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			ASC->InitAbilityActorInfo(this, this);
			InitializeClass(GetClassId());

			ASC->GetGameplayAttributeValueChangeDelegate(UArenaAttributeSet::GetHealthAttribute())
				.AddLambda([this](const FOnAttributeChangeData& Change)
				{
					if (Change.NewValue < Change.OldValue)
					{
						LastDamageTime = GetWorld()->GetTimeSeconds();
					}
				});

			GetWorldTimerManager().SetTimer(
				ResetTimerHandle, this, &AArenaTrainingDummy::CheckHealthReset, 1.f, true);
		}
	}
	else if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->InitAbilityActorInfo(this, this);
	}
}

void AArenaTrainingDummy::CheckHealthReset()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	const UArenaAttributeSet* DummyAttributes = ASC ? ASC->GetSet<UArenaAttributeSet>() : nullptr;
	if (!DummyAttributes)
	{
		return;
	}

	const bool bDamaged = DummyAttributes->GetHealth() < DummyAttributes->GetMaxHealth();
	const bool bQuietPeriodElapsed =
		LastDamageTime < 0.f || (GetWorld()->GetTimeSeconds() - LastDamageTime) >= HealthResetDelaySec;

	if (bDamaged && bQuietPeriodElapsed)
	{
		ASC->SetNumericAttributeBase(UArenaAttributeSet::GetHealthAttribute(), DummyAttributes->GetMaxHealth());
	}
}
