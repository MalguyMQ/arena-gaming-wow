// Copyright Flagcat Studio. All Rights Reserved.

#include "Player/ArenaInputSetup.h"
#include "ArenaCore.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Misc/ConfigCacheIni.h"

namespace
{
	FString ReadIniString(const FConfigFile& Ini, const TCHAR* Section, const TCHAR* Key, const TCHAR* Fallback)
	{
		FString Value;
		if (!Ini.GetString(Section, Key, Value) || Value.IsEmpty())
		{
			Value = Fallback;
		}
		return Value;
	}

	FKey ReadIniKey(const FConfigFile& Ini, const TCHAR* Section, const TCHAR* KeyName, const TCHAR* Fallback)
	{
		const FString KeyString = ReadIniString(Ini, Section, KeyName, Fallback);
		FKey Key(*KeyString);
		if (!Key.IsValid())
		{
			UE_LOG(LogArena, Warning, TEXT("Keybind '%s' : touche inconnue '%s', retour à '%s'"), KeyName, *KeyString, Fallback);
			Key = FKey(Fallback);
		}
		return Key;
	}
}

void UArenaInputSetup::Build(APlayerController* PlayerController)
{
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		return;
	}

	FConfigFile Ini;
	FConfigCacheIni::LoadLocalIniFile(Ini, TEXT("ArenaKeybinds"), /*bIsBaseIniName*/ true);

	MouseSensitivity = FCString::Atof(*ReadIniString(Ini, TEXT("Camera"), TEXT("MouseSensitivity"), TEXT("1.0")));
	if (MouseSensitivity <= 0.f)
	{
		MouseSensitivity = 1.f;
	}
	bInvertY = ReadIniString(Ini, TEXT("Camera"), TEXT("InvertY"), TEXT("False")).ToBool();
	ZoomStep = FMath::Max(10.f, FCString::Atof(*ReadIniString(Ini, TEXT("Camera"), TEXT("ZoomStep"), TEXT("50"))));

	auto MakeAction = [this](EInputActionValueType ValueType)
	{
		UInputAction* Action = NewObject<UInputAction>(this);
		Action->ValueType = ValueType;
		return Action;
	};

	IA_MoveForward = MakeAction(EInputActionValueType::Axis1D);
	IA_Strafe = MakeAction(EInputActionValueType::Axis1D);
	IA_Turn = MakeAction(EInputActionValueType::Axis1D);
	IA_Jump = MakeAction(EInputActionValueType::Boolean);
	IA_Look = MakeAction(EInputActionValueType::Axis2D);
	IA_Zoom = MakeAction(EInputActionValueType::Axis1D);
	IA_TargetCycle = MakeAction(EInputActionValueType::Boolean);

	IMC_Default = NewObject<UInputMappingContext>(this);

	auto MapPositive = [this](UInputAction* Action, const FKey& Key)
	{
		IMC_Default->MapKey(Action, Key);
	};
	auto MapNegative = [this](UInputAction* Action, const FKey& Key)
	{
		FEnhancedActionKeyMapping& Mapping = IMC_Default->MapKey(Action, Key);
		Mapping.Modifiers.Add(NewObject<UInputModifierNegate>(this));
	};

	// Défauts AZERTY (public FR) — voir DefaultArenaKeybinds.ini pour rebinder.
	MapPositive(IA_MoveForward, ReadIniKey(Ini, TEXT("Movement"), TEXT("Forward"), TEXT("Z")));
	MapNegative(IA_MoveForward, ReadIniKey(Ini, TEXT("Movement"), TEXT("Backward"), TEXT("S")));
	MapPositive(IA_Strafe, ReadIniKey(Ini, TEXT("Movement"), TEXT("StrafeRight"), TEXT("D")));
	MapNegative(IA_Strafe, ReadIniKey(Ini, TEXT("Movement"), TEXT("StrafeLeft"), TEXT("Q")));
	MapPositive(IA_Turn, ReadIniKey(Ini, TEXT("Movement"), TEXT("TurnRight"), TEXT("E")));
	MapNegative(IA_Turn, ReadIniKey(Ini, TEXT("Movement"), TEXT("TurnLeft"), TEXT("A")));
	MapPositive(IA_Jump, ReadIniKey(Ini, TEXT("Movement"), TEXT("Jump"), TEXT("SpaceBar")));
	MapPositive(IA_TargetCycle, ReadIniKey(Ini, TEXT("Targeting"), TEXT("CycleTarget"), TEXT("Tab")));
	MapPositive(IA_Look, EKeys::Mouse2D);
	MapPositive(IA_Zoom, EKeys::MouseWheelAxis);

	static const TCHAR* DefaultSlotKeys[] =
	{
		TEXT("One"), TEXT("Two"), TEXT("Three"), TEXT("Four"),
		TEXT("Five"), TEXT("Six"), TEXT("Seven"), TEXT("Eight")
	};
	IA_ActionSlots.Reset();
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(DefaultSlotKeys); ++Index)
	{
		UInputAction* SlotAction = MakeAction(EInputActionValueType::Boolean);
		const FString IniKeyName = FString::Printf(TEXT("Slot%d"), Index + 1);
		MapPositive(SlotAction, ReadIniKey(Ini, TEXT("ActionBar"), *IniKeyName, DefaultSlotKeys[Index]));
		IA_ActionSlots.Add(SlotAction);
	}

	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(IMC_Default, 0);
			UE_LOG(LogArena, Log, TEXT("Input runtime construit et mapping context appliqué."));
		}
	}
}
