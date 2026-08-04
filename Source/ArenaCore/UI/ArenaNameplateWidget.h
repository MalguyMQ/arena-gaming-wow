// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArenaNameplateWidget.generated.h"

class UTextBlock;
class UProgressBar;
class AArenaCharacter;

// Nameplate au-dessus des personnages (nom + barre de vie), arbre construit
// en C++ — aucun asset UMG. Attaché via UWidgetComponent en espace écran.
UCLASS()
class ARENACORE_API UArenaNameplateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetObservedCharacter(AArenaCharacter* InCharacter);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY() TObjectPtr<UTextBlock> NameText;
	UPROPERTY() TObjectPtr<UProgressBar> HealthBar;

	TWeakObjectPtr<AArenaCharacter> Observed;
};
