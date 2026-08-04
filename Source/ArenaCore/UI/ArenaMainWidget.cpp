// Copyright Flagcat Studio. All Rights Reserved.

#include "UI/ArenaMainWidget.h"
#include "Characters/ArenaCharacter.h"
#include "Combat/ArenaTargetingComponent.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "ArenaGameplayTags.h"
#include "System/ArenaDataSubsystem.h"
#include "System/ArenaDataRows.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/GameInstance.h"
#include "Styling/CoreStyle.h"

void UArenaMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>();
	WidgetTree->RootWidget = RootCanvas;

	PlayerFrame = BuildUnitFrame(RootCanvas, FVector2D(40.f, 40.f));
	TargetFrame = BuildUnitFrame(RootCanvas, FVector2D(340.f, 40.f));

	// Barre d'action : 8 slots ancrés en bas au centre.
	UHorizontalBox* Bar = WidgetTree->ConstructWidget<UHorizontalBox>();
	UCanvasPanelSlot* BarSlot = RootCanvas->AddChildToCanvas(Bar);
	BarSlot->SetAnchors(FAnchors(0.5f, 1.f, 0.5f, 1.f));
	BarSlot->SetAlignment(FVector2D(0.5f, 1.f));
	BarSlot->SetPosition(FVector2D(0.f, -40.f));
	BarSlot->SetAutoSize(true);

	ActionSlots.Reset();
	for (int32 Index = 0; Index < NumActionSlots; ++Index)
	{
		FActionSlotWidgets SlotWidgets;

		SlotWidgets.Background = WidgetTree->ConstructWidget<UBorder>();
		SlotWidgets.Background->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.1f, 0.9f));
		SlotWidgets.Background->SetPadding(FMargin(6.f));

		UOverlay* SlotOverlay = WidgetTree->ConstructWidget<UOverlay>();
		SlotWidgets.Background->SetContent(SlotOverlay);

		UVerticalBox* SlotLayout = WidgetTree->ConstructWidget<UVerticalBox>();
		SlotOverlay->AddChildToOverlay(SlotLayout);

		UTextBlock* KeyText = WidgetTree->ConstructWidget<UTextBlock>();
		KeyText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 9));
		KeyText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
		KeyText->SetText(FText::AsNumber(Index + 1));
		SlotLayout->AddChildToVerticalBox(KeyText);

		SlotWidgets.Label = WidgetTree->ConstructWidget<UTextBlock>();
		SlotWidgets.Label->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 10));
		SlotWidgets.Label->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		SlotLayout->AddChildToVerticalBox(SlotWidgets.Label);

		SlotWidgets.Cooldown = WidgetTree->ConstructWidget<UTextBlock>();
		SlotWidgets.Cooldown->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 18));
		SlotWidgets.Cooldown->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.85f, 0.2f)));
		SlotWidgets.Cooldown->SetJustification(ETextJustify::Center);
		UOverlaySlot* CooldownSlot = SlotOverlay->AddChildToOverlay(SlotWidgets.Cooldown);
		CooldownSlot->SetHorizontalAlignment(HAlign_Center);
		CooldownSlot->SetVerticalAlignment(VAlign_Center);

		UHorizontalBoxSlot* InBarSlot = Bar->AddChildToHorizontalBox(SlotWidgets.Background);
		InBarSlot->SetPadding(FMargin(2.f, 0.f));

		ActionSlots.Add(SlotWidgets);
	}

	FeedbackHandle = AArenaCharacter::OnCombatFeedback.AddUObject(this, &UArenaMainWidget::HandleCombatFeedback);
}

void UArenaMainWidget::NativeDestruct()
{
	AArenaCharacter::OnCombatFeedback.Remove(FeedbackHandle);
	Super::NativeDestruct();
}

UArenaMainWidget::FUnitFrame UArenaMainWidget::BuildUnitFrame(UCanvasPanel* Canvas, const FVector2D& Position)
{
	FUnitFrame Frame;

	UBorder* Background = WidgetTree->ConstructWidget<UBorder>();
	Background->SetBrushColor(FLinearColor(0.05f, 0.05f, 0.07f, 0.8f));
	Background->SetPadding(FMargin(8.f));

	UCanvasPanelSlot* FrameSlot = Canvas->AddChildToCanvas(Background);
	FrameSlot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
	FrameSlot->SetPosition(Position);
	FrameSlot->SetSize(FVector2D(240.f, 84.f));

	Frame.Box = WidgetTree->ConstructWidget<UVerticalBox>();
	Background->SetContent(Frame.Box);

	Frame.Name = WidgetTree->ConstructWidget<UTextBlock>();
	Frame.Name->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 12));
	Frame.Name->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	Frame.Box->AddChildToVerticalBox(Frame.Name);

	Frame.Health = WidgetTree->ConstructWidget<UProgressBar>();
	Frame.Health->SetFillColorAndOpacity(FLinearColor(0.15f, 0.8f, 0.25f));
	UVerticalBoxSlot* HealthSlot = Frame.Box->AddChildToVerticalBox(Frame.Health);
	HealthSlot->SetPadding(FMargin(0.f, 4.f, 0.f, 0.f));

	Frame.HealthText = WidgetTree->ConstructWidget<UTextBlock>();
	Frame.HealthText->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 10));
	Frame.HealthText->SetColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.85f, 0.85f)));
	Frame.Box->AddChildToVerticalBox(Frame.HealthText);

	Frame.Resource = WidgetTree->ConstructWidget<UProgressBar>();
	Frame.Resource->SetFillColorAndOpacity(FLinearColor(0.2f, 0.4f, 0.95f));
	UVerticalBoxSlot* ResourceSlot = Frame.Box->AddChildToVerticalBox(Frame.Resource);
	ResourceSlot->SetPadding(FMargin(0.f, 4.f, 0.f, 0.f));

	return Frame;
}

void UArenaMainWidget::UpdateUnitFrame(const FUnitFrame& Frame, const AArenaCharacter* Character) const
{
	const bool bVisible = Character != nullptr;
	if (Frame.Box)
	{
		if (UWidget* FrameRoot = Frame.Box->GetParent())
		{
			FrameRoot->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
		}
	}
	if (!bVisible)
	{
		return;
	}

	const UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	Frame.Name->SetText(Character->GetDisplayName());

	const float Health = ASC->GetNumericAttribute(UArenaAttributeSet::GetHealthAttribute());
	const float MaxHealth = ASC->GetNumericAttribute(UArenaAttributeSet::GetMaxHealthAttribute());
	Frame.Health->SetPercent(MaxHealth > 0.f ? Health / MaxHealth : 0.f);
	Frame.HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));

	const float MaxMana = ASC->GetNumericAttribute(UArenaAttributeSet::GetMaxManaAttribute());
	const float MaxRage = ASC->GetNumericAttribute(UArenaAttributeSet::GetMaxRageAttribute());
	const float MaxEnergy = ASC->GetNumericAttribute(UArenaAttributeSet::GetMaxEnergyAttribute());

	if (MaxMana > 0.f)
	{
		Frame.Resource->SetVisibility(ESlateVisibility::HitTestInvisible);
		Frame.Resource->SetFillColorAndOpacity(FLinearColor(0.2f, 0.4f, 0.95f));
		Frame.Resource->SetPercent(ASC->GetNumericAttribute(UArenaAttributeSet::GetManaAttribute()) / MaxMana);
	}
	else if (MaxRage > 0.f)
	{
		Frame.Resource->SetVisibility(ESlateVisibility::HitTestInvisible);
		Frame.Resource->SetFillColorAndOpacity(FLinearColor(0.85f, 0.15f, 0.15f));
		Frame.Resource->SetPercent(ASC->GetNumericAttribute(UArenaAttributeSet::GetRageAttribute()) / MaxRage);
	}
	else if (MaxEnergy > 0.f)
	{
		Frame.Resource->SetVisibility(ESlateVisibility::HitTestInvisible);
		Frame.Resource->SetFillColorAndOpacity(FLinearColor(0.95f, 0.9f, 0.3f));
		Frame.Resource->SetPercent(ASC->GetNumericAttribute(UArenaAttributeSet::GetEnergyAttribute()) / MaxEnergy);
	}
	else
	{
		Frame.Resource->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UArenaMainWidget::UpdateActionBar(const AArenaCharacter* Character)
{
	const UAbilitySystemComponent* ASC = Character ? Character->GetAbilitySystemComponent() : nullptr;
	const UGameInstance* GI = Character ? Character->GetGameInstance() : nullptr;
	UArenaDataSubsystem* Data = GI ? GI->GetSubsystem<UArenaDataSubsystem>() : nullptr;

	float GCDRemaining = 0.f;
	if (ASC)
	{
		const FGameplayEffectQuery GCDQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(
			FGameplayTagContainer(ArenaTags::Cooldown_GCD));
		for (const float Remaining : ASC->GetActiveEffectsTimeRemaining(GCDQuery))
		{
			GCDRemaining = FMath::Max(GCDRemaining, Remaining);
		}
	}

	for (int32 Index = 0; Index < ActionSlots.Num(); ++Index)
	{
		const FActionSlotWidgets& SlotWidgets = ActionSlots[Index];
		const int32 SlotNumber = Index + 1;

		FText Label = FText::GetEmpty();
		if (Character && Data)
		{
			const FName RowName = Character->GetAbilityRowForSlot(SlotNumber);
			if (const FAbilityRow* Row = Data->FindRow<FAbilityRow>(UArenaDataSubsystem::TableId_Abilities, RowName))
			{
				Label = FText::FromString(Row->DisplayName);
			}
		}
		SlotWidgets.Label->SetText(Label);

		float SlotRemaining = 0.f;
		if (ASC)
		{
			const FGameplayEffectQuery SlotQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(
				FGameplayTagContainer(ArenaTags::CooldownSlotTag(SlotNumber)));
			for (const float Remaining : ASC->GetActiveEffectsTimeRemaining(SlotQuery))
			{
				SlotRemaining = FMath::Max(SlotRemaining, Remaining);
			}
		}

		const float DisplayRemaining = FMath::Max(SlotRemaining, GCDRemaining);
		if (DisplayRemaining > 0.05f)
		{
			SlotWidgets.Cooldown->SetText(FText::AsNumber(FMath::CeilToInt(DisplayRemaining)));
			SlotWidgets.Background->SetBrushColor(FLinearColor(0.03f, 0.03f, 0.04f, 0.9f));
		}
		else
		{
			SlotWidgets.Cooldown->SetText(FText::GetEmpty());
			SlotWidgets.Background->SetBrushColor(FLinearColor(0.08f, 0.08f, 0.1f, 0.9f));
		}
	}
}

void UArenaMainWidget::HandleCombatFeedback(AActor* Victim, float Magnitude, int32 FeedbackType)
{
	if (!Victim || !RootCanvas || FloatingTexts.Num() >= MaxFloatingTexts)
	{
		return;
	}

	FFloatingText Entry;
	Entry.Text = WidgetTree->ConstructWidget<UTextBlock>();
	Entry.Text->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 18));
	Entry.Text->SetColorAndOpacity(FSlateColor(FeedbackType == 1
		? FLinearColor(0.3f, 1.f, 0.35f)
		: FLinearColor(1.f, 0.25f, 0.2f)));
	Entry.Text->SetText(FText::AsNumber(FMath::RoundToInt(Magnitude)));

	UCanvasPanelSlot* TextSlot = RootCanvas->AddChildToCanvas(Entry.Text);
	TextSlot->SetAutoSize(true);

	Entry.WorldLocation = Victim->GetActorLocation()
		+ FVector(FMath::FRandRange(-30.f, 30.f), FMath::FRandRange(-30.f, 30.f), 110.f);
	Entry.SpawnTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	FloatingTexts.Add(Entry);
}

void UArenaMainWidget::UpdateFloatingTexts(float Now)
{
	APlayerController* PC = GetOwningPlayer();
	for (int32 Index = FloatingTexts.Num() - 1; Index >= 0; --Index)
	{
		FFloatingText& Entry = FloatingTexts[Index];
		const float Age = Now - Entry.SpawnTime;
		if (Age > FloatingTextLifetime || !Entry.Text)
		{
			if (Entry.Text)
			{
				Entry.Text->RemoveFromParent();
			}
			FloatingTexts.RemoveAt(Index);
			continue;
		}

		FVector2D ScreenPosition;
		if (PC && UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, Entry.WorldLocation, ScreenPosition, false))
		{
			if (UCanvasPanelSlot* TextSlot = Cast<UCanvasPanelSlot>(Entry.Text->Slot))
			{
				TextSlot->SetPosition(ScreenPosition + FVector2D(0.f, -60.f * Age));
			}
			Entry.Text->SetRenderOpacity(1.f - Age / FloatingTextLifetime);
			Entry.Text->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Entry.Text->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UArenaMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const AArenaCharacter* OwnCharacter = Cast<AArenaCharacter>(GetOwningPlayerPawn());
	UpdateUnitFrame(PlayerFrame, OwnCharacter);

	const AArenaCharacter* Target = nullptr;
	if (OwnCharacter && OwnCharacter->GetTargetingComponent())
	{
		Target = Cast<AArenaCharacter>(OwnCharacter->GetTargetingComponent()->GetCurrentTarget());
	}
	UpdateUnitFrame(TargetFrame, Target);

	UpdateActionBar(OwnCharacter);
	UpdateFloatingTexts(GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f);
}
