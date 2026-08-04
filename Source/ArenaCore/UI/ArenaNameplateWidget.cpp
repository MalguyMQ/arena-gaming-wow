// Copyright Flagcat Studio. All Rights Reserved.

#include "UI/ArenaNameplateWidget.h"
#include "Characters/ArenaCharacter.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Styling/CoreStyle.h"

void UArenaNameplateWidget::SetObservedCharacter(AArenaCharacter* InCharacter)
{
	Observed = InCharacter;
}

void UArenaNameplateWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	USizeBox* Root = WidgetTree->ConstructWidget<USizeBox>();
	Root->SetWidthOverride(160.f);
	WidgetTree->RootWidget = Root;

	UVerticalBox* Layout = WidgetTree->ConstructWidget<UVerticalBox>();
	Root->AddChild(Layout);

	NameText = WidgetTree->ConstructWidget<UTextBlock>();
	NameText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 11));
	NameText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	NameText->SetJustification(ETextJustify::Center);
	Layout->AddChildToVerticalBox(NameText);

	HealthBar = WidgetTree->ConstructWidget<UProgressBar>();
	HealthBar->SetFillColorAndOpacity(FLinearColor(0.15f, 0.8f, 0.25f));
	Layout->AddChildToVerticalBox(HealthBar);
}

void UArenaNameplateWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const AArenaCharacter* Character = Observed.Get();
	if (!Character || !NameText || !HealthBar)
	{
		return;
	}

	NameText->SetText(Character->GetDisplayName());

	if (const UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
	{
		const float Health = ASC->GetNumericAttribute(UArenaAttributeSet::GetHealthAttribute());
		const float MaxHealth = ASC->GetNumericAttribute(UArenaAttributeSet::GetMaxHealthAttribute());
		HealthBar->SetPercent(MaxHealth > 0.f ? Health / MaxHealth : 0.f);
	}
}
