// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArenaMainWidget.generated.h"

class UCanvasPanel;
class UTextBlock;
class UProgressBar;
class UBorder;
class UVerticalBox;
class AArenaCharacter;

// HUD principal construit 100 % en C++ (aucun asset UMG) :
// frame joueur (PV + ressource), frame de cible, barre d'action avec décomptes
// de cooldown, et combat text flottant alimenté par AArenaCharacter::OnCombatFeedback.
// Rafraîchi par polling des attributs — trivial à cette échelle et toujours juste.
UCLASS()
class ARENACORE_API UArenaMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	struct FUnitFrame
	{
		UVerticalBox* Box = nullptr;
		UTextBlock* Name = nullptr;
		UProgressBar* Health = nullptr;
		UTextBlock* HealthText = nullptr;
		UProgressBar* Resource = nullptr;
	};

	struct FActionSlotWidgets
	{
		UBorder* Background = nullptr;
		UTextBlock* Label = nullptr;
		UTextBlock* Cooldown = nullptr;
	};

	struct FFloatingText
	{
		UTextBlock* Text = nullptr;
		FVector WorldLocation = FVector::ZeroVector;
		float SpawnTime = 0.f;
	};

	FUnitFrame BuildUnitFrame(UCanvasPanel* Canvas, const FVector2D& Position);
	void UpdateUnitFrame(const FUnitFrame& Frame, const AArenaCharacter* Character) const;
	void UpdateActionBar(const AArenaCharacter* Character);
	void UpdateFloatingTexts(float Now);
	void HandleCombatFeedback(AActor* Victim, float Magnitude, int32 FeedbackType);

	UPROPERTY() TObjectPtr<UCanvasPanel> RootCanvas;

	FUnitFrame PlayerFrame;
	FUnitFrame TargetFrame;
	TArray<FActionSlotWidgets> ActionSlots;
	TArray<FFloatingText> FloatingTexts;
	FDelegateHandle FeedbackHandle;

	static constexpr int32 NumActionSlots = 8;
	static constexpr float FloatingTextLifetime = 1.2f;
	static constexpr int32 MaxFloatingTexts = 30;
};
