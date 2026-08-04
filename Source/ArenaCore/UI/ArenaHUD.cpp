// Copyright Flagcat Studio. All Rights Reserved.

#include "UI/ArenaHUD.h"
#include "UI/ArenaMainWidget.h"
#include "Blueprint/UserWidget.h"

void AArenaHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerOwner && PlayerOwner->IsLocalController())
	{
		MainWidget = CreateWidget<UArenaMainWidget>(PlayerOwner.Get(), UArenaMainWidget::StaticClass());
		if (MainWidget)
		{
			MainWidget->AddToViewport();
		}
	}
}
