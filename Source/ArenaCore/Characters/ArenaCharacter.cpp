// Copyright Flagcat Studio. All Rights Reserved.

#include "Characters/ArenaCharacter.h"
#include "Characters/ArenaCharacterMovementComponent.h"
#include "Player/ArenaPlayerController.h"
#include "Player/ArenaPlayerState.h"
#include "Player/ArenaInputSetup.h"
#include "AbilitySystem/ArenaAbilitySystemComponent.h"
#include "AbilitySystem/ArenaAttributeSet.h"
#include "AbilitySystem/Abilities/ArenaAbility_InstantDamage.h"
#include "AbilitySystem/Abilities/ArenaAbility_CastedSpell.h"
#include "AbilitySystem/Abilities/ArenaAbility_Interrupt.h"
#include "GameFramework/GameStateBase.h"
#include "Combat/ArenaTargetingComponent.h"
#include "System/ArenaDataSubsystem.h"
#include "System/ArenaDataRows.h"
#include "UI/ArenaNameplateWidget.h"
#include "ArenaCore.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

FArenaCombatFeedbackDelegate AArenaCharacter::OnCombatFeedback;

static FAutoConsoleCommandWithWorldAndArgs GArenaSetClassCmd(
	TEXT("Arena.SetClass"),
	TEXT("Change la classe du personnage local : Arena.SetClass Warrior|Mage|Priest|Rogue"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		if (!World || Args.Num() == 0)
		{
			return;
		}
		APlayerController* PC = World->GetFirstPlayerController();
		if (AArenaCharacter* Character = PC ? Cast<AArenaCharacter>(PC->GetPawn()) : nullptr)
		{
			Character->ServerSetClass(FName(*Args[0]));
		}
	}));

AArenaCharacter::AArenaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UArenaCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	AbilitySystem = CreateDefaultSubobject<UArenaAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	Attributes = CreateDefaultSubobject<UArenaAttributeSet>(TEXT("Attributes"));
	Targeting = CreateDefaultSubobject<UArenaTargetingComponent>(TEXT("Targeting"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	SpringArm->TargetArmLength = 450.f;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = false;
	SpringArm->bDoCollisionTest = true;
	SpringArm->bEnableCameraLag = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	// Exposition fixe : pas de pompage de luminosité sur une scène greybox.
	Camera->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	Camera->PostProcessSettings.AutoExposureMinBrightness = 1.f;
	Camera->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
	Camera->PostProcessSettings.AutoExposureMaxBrightness = 1.f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleMeshFinder(TEXT("/Engine/BasicShapes/Capsule.Capsule"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
	if (CapsuleMeshFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(CapsuleMeshFinder.Object);
	}
	BodyMesh->SetRelativeScale3D(FVector(0.68f, 0.68f, 0.88f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FacingMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FacingMarker"));
	FacingMarker->SetupAttachment(RootComponent);
	if (CubeMeshFinder.Succeeded())
	{
		FacingMarker->SetStaticMesh(CubeMeshFinder.Object);
	}
	FacingMarker->SetRelativeLocation(FVector(45.f, 0.f, 55.f));
	FacingMarker->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	FacingMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AbilitySlotRows.Init(NAME_None, 12);
}

void AArenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArenaCharacter, AbilitySlotRows);
	DOREPLIFETIME(AArenaCharacter, CastState);
}

void AArenaCharacter::SetCastState(FName AbilityRow, float Duration)
{
	if (!HasAuthority())
	{
		return;
	}
	const AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	const float ServerNow = GS ? GS->GetServerWorldTimeSeconds() : (GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f);
	CastState.AbilityRow = AbilityRow;
	CastState.StartServerTime = ServerNow;
	CastState.EndServerTime = ServerNow + Duration;
}

void AArenaCharacter::ClearCastState()
{
	if (HasAuthority())
	{
		CastState = FArenaCastState();
	}
}

void AArenaCharacter::NotifyCombatActivity()
{
	if (HasAuthority() && GetWorld())
	{
		LastCombatTime = GetWorld()->GetTimeSeconds();
	}
}

UAbilitySystemComponent* AArenaCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

FName AArenaCharacter::GetClassId() const
{
	const AArenaPlayerState* PS = GetPlayerState<AArenaPlayerState>();
	return PS ? PS->ClassId : FName("Warrior");
}

FText AArenaCharacter::GetDisplayName() const
{
	const APlayerState* PS = GetPlayerState();
	return PS ? FText::FromString(PS->GetPlayerName()) : NSLOCTEXT("Arena", "UnknownUnit", "Inconnu");
}

void AArenaCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystem)
	{
		AbilitySystem->InitAbilityActorInfo(this, this);
	}
	if (HasAuthority())
	{
		InitializeClass(GetClassId());
	}
}

void AArenaCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	if (AbilitySystem)
	{
		AbilitySystem->InitAbilityActorInfo(this, this);
	}
}

void AArenaCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AbilitySystem)
	{
		AbilitySystem->InitAbilityActorInfo(this, this);
	}
}

void AArenaCharacter::InitializeClass(FName InClassId)
{
	if (!HasAuthority() || !AbilitySystem || !Attributes)
	{
		return;
	}

	const UGameInstance* GI = GetGameInstance();
	UArenaDataSubsystem* Data = GI ? GI->GetSubsystem<UArenaDataSubsystem>() : nullptr;
	const FStatTemplateRow* Stats = Data
		? Data->FindRow<FStatTemplateRow>(UArenaDataSubsystem::TableId_StatTemplates, InClassId)
		: nullptr;
	if (!Stats)
	{
		UE_LOG(LogArena, Error, TEXT("InitializeClass : template de stats introuvable pour '%s'"), *InClassId.ToString());
		return;
	}

	Attributes->SetMaxHealth(Stats->MaxHealth);
	Attributes->SetHealth(Stats->MaxHealth);
	Attributes->SetMaxMana(Stats->MaxMana);
	Attributes->SetMana(Stats->MaxMana);
	Attributes->SetMaxRage(Stats->MaxRage);
	Attributes->SetRage(0.f);
	Attributes->SetMaxEnergy(Stats->MaxEnergy);
	Attributes->SetEnergy(Stats->MaxEnergy);
	Attributes->SetComboPoints(0.f);
	Attributes->SetAttackPower(Stats->AttackPower);
	Attributes->SetSpellPower(Stats->SpellPower);
	Attributes->SetHastePct(Stats->HastePct);
	Attributes->SetCritPct(Stats->CritPct);
	Attributes->SetDamageTakenMult(1.f);
	Attributes->SetHealingTakenMult(1.f);
	Attributes->SetAbsorb(0.f);

	AbilitySlotRows.Init(NAME_None, 12);
	AbilitySystem->ClearAllAbilities();

	const UDataTable* AbilityTable = Data->GetTable(UArenaDataSubsystem::TableId_Abilities);
	if (!AbilityTable)
	{
		return;
	}

	CachedManaRegenPerSec = Stats->ManaRegenPerSec;
	CachedEnergyRegenPerSec = Stats->EnergyRegenPerSec;

	// Un verbe = une classe C++ générique. Les lignes dont le verbe n'est pas
	// encore implémenté occupent leur slot (label UI) sans être données.
	auto VerbClass = [](FName Verb) -> TSubclassOf<UGameplayAbility>
	{
		if (Verb == FName("InstantDamage")) { return UArenaAbility_InstantDamage::StaticClass(); }
		if (Verb == FName("CastedSpell")) { return UArenaAbility_CastedSpell::StaticClass(); }
		if (Verb == FName("Interrupt")) { return UArenaAbility_Interrupt::StaticClass(); }
		return nullptr;
	};

	int32 GrantedCount = 0;
	for (const TPair<FName, uint8*>& Pair : AbilityTable->GetRowMap())
	{
		const FAbilityRow* Row = reinterpret_cast<const FAbilityRow*>(Pair.Value);
		if (!Row || Row->ClassId != InClassId)
		{
			continue;
		}
		if (Row->Slot < 0 || Row->Slot >= AbilitySlotRows.Num())
		{
			continue;
		}

		AbilitySlotRows[Row->Slot] = Pair.Key;
		if (const TSubclassOf<UGameplayAbility> AbilityClass = VerbClass(Row->Verb))
		{
			AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, Row->Slot));
			++GrantedCount;
		}
	}

	UE_LOG(LogArena, Log, TEXT("Classe '%s' initialisée : %d sorts actifs."), *InClassId.ToString(), GrantedCount);
}

void AArenaCharacter::ServerSetClass_Implementation(FName InClassId)
{
	if (AArenaPlayerState* PS = GetPlayerState<AArenaPlayerState>())
	{
		PS->ClassId = InClassId;
	}
	InitializeClass(InClassId);
}

FName AArenaCharacter::GetAbilityRowForSlot(int32 Slot) const
{
	return AbilitySlotRows.IsValidIndex(Slot) ? AbilitySlotRows[Slot] : NAME_None;
}

void AArenaCharacter::MulticastCombatFeedback_Implementation(float Magnitude, int32 FeedbackType)
{
	OnCombatFeedback.Broadcast(this, Magnitude, FeedbackType);
}

void AArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UMaterialInstanceDynamic* BodyMID = BodyMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		BodyMID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.2f, 0.45f, 0.9f));
	}
	if (UMaterialInstanceDynamic* MarkerMID = FacingMarker->CreateAndSetMaterialInstanceDynamic(0))
	{
		MarkerMID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.05f, 0.05f, 0.05f));
	}

	CreateNameplate();
}

void AArenaCharacter::CreateNameplate()
{
	if (GetNetMode() == NM_DedicatedServer || NameplateComponent)
	{
		return;
	}

	NameplateComponent = NewObject<UWidgetComponent>(this, TEXT("Nameplate"));
	NameplateComponent->SetupAttachment(RootComponent);
	NameplateComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NameplateComponent->SetDrawAtDesiredSize(true);
	NameplateComponent->SetWidgetClass(UArenaNameplateWidget::StaticClass());
	NameplateComponent->SetRelativeLocation(FVector(0.f, 0.f, 130.f));
	NameplateComponent->RegisterComponent();
	NameplateComponent->InitWidget();
	if (UArenaNameplateWidget* Plate = Cast<UArenaNameplateWidget>(NameplateComponent->GetWidget()))
	{
		Plate->SetObservedCharacter(this);
	}
}

void AArenaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Serveur : régénération des ressources (mana/énergie passives, rage en
	// combat qui décroît hors combat — fenêtre de 6 s après la dernière action).
	if (HasAuthority() && Attributes && AbilitySystem)
	{
		if (CachedManaRegenPerSec > 0.f)
		{
			Attributes->SetMana(Attributes->GetMana() + CachedManaRegenPerSec * DeltaSeconds);
		}
		if (CachedEnergyRegenPerSec > 0.f)
		{
			Attributes->SetEnergy(Attributes->GetEnergy() + CachedEnergyRegenPerSec * DeltaSeconds);
		}
		if (Attributes->GetMaxRage() > 0.f)
		{
			const bool bInCombat = (GetWorld()->GetTimeSeconds() - LastCombatTime) < CombatWindowSec;
			const float RageDelta = bInCombat ? RageInCombatPerSec : -RageDecayPerSec;
			Attributes->SetRage(Attributes->GetRage() + RageDelta * DeltaSeconds);
		}
	}

	if (NameplateComponent)
	{
		NameplateComponent->SetVisibility(!IsLocallyControlled());
	}

	if (!IsLocallyControlled())
	{
		return;
	}

	const bool bRightMouseDown = IsRightMouseDown();
	if (bRightMouseDown && !bMouselookHeld)
	{
		// WoW : presser le clic droit aligne le personnage sur la direction de la caméra.
		if (Controller)
		{
			FRotator ControlRotation = Controller->GetControlRotation();
			ControlRotation.Yaw += CameraYawOffset;
			Controller->SetControlRotation(ControlRotation);
		}
		CameraYawOffset = 0.f;
	}
	bMouselookHeld = bRightMouseDown;

	// WoW : les deux boutons de la souris maintenus = avancer (en steerant à la souris).
	if (bRightMouseDown && IsLeftMouseDown())
	{
		AddMovementInput(GetActorForwardVector(), 1.f);
	}

	if (SpringArm)
	{
		SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYawOffset, 0.f));
	}
}

void AArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	AArenaPlayerController* ArenaPC = Cast<AArenaPlayerController>(GetController());
	if (!ArenaPC || !ArenaPC->IsLocalController())
	{
		return;
	}

	UArenaInputSetup* Setup = ArenaPC->GetInputSetup();
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Setup || !EnhancedInput)
	{
		UE_LOG(LogArena, Error, TEXT("Input non initialisé : Setup=%d EnhancedInput=%d"), Setup != nullptr, EnhancedInput != nullptr);
		return;
	}

	EnhancedInput->BindAction(Setup->IA_MoveForward, ETriggerEvent::Triggered, this, &AArenaCharacter::Input_MoveForward);
	EnhancedInput->BindAction(Setup->IA_Strafe, ETriggerEvent::Triggered, this, &AArenaCharacter::Input_Strafe);
	EnhancedInput->BindAction(Setup->IA_Turn, ETriggerEvent::Triggered, this, &AArenaCharacter::Input_Turn);
	EnhancedInput->BindAction(Setup->IA_Look, ETriggerEvent::Triggered, this, &AArenaCharacter::Input_Look);
	EnhancedInput->BindAction(Setup->IA_Zoom, ETriggerEvent::Triggered, this, &AArenaCharacter::Input_Zoom);
	EnhancedInput->BindAction(Setup->IA_Jump, ETriggerEvent::Started, this, &AArenaCharacter::Input_JumpStarted);
	EnhancedInput->BindAction(Setup->IA_Jump, ETriggerEvent::Completed, this, &AArenaCharacter::Input_JumpCompleted);
	EnhancedInput->BindAction(Setup->IA_TargetCycle, ETriggerEvent::Started, this, &AArenaCharacter::Input_CycleTarget);

	static const TArray<void (AArenaCharacter::*)(const FInputActionValue&)> SlotHandlers =
	{
		&AArenaCharacter::Input_Slot1, &AArenaCharacter::Input_Slot2,
		&AArenaCharacter::Input_Slot3, &AArenaCharacter::Input_Slot4,
		&AArenaCharacter::Input_Slot5, &AArenaCharacter::Input_Slot6,
		&AArenaCharacter::Input_Slot7, &AArenaCharacter::Input_Slot8
	};
	for (int32 Index = 0; Index < Setup->IA_ActionSlots.Num() && Index < SlotHandlers.Num(); ++Index)
	{
		if (Setup->IA_ActionSlots[Index])
		{
			EnhancedInput->BindAction(Setup->IA_ActionSlots[Index], ETriggerEvent::Started, this, SlotHandlers[Index]);
		}
	}
}

void AArenaCharacter::Input_MoveForward(const FInputActionValue& Value)
{
	if (Controller)
	{
		AddMovementInput(GetActorForwardVector(), Value.Get<float>());
	}
}

void AArenaCharacter::Input_Strafe(const FInputActionValue& Value)
{
	if (Controller)
	{
		AddMovementInput(GetActorRightVector(), Value.Get<float>());
	}
}

void AArenaCharacter::Input_Turn(const FInputActionValue& Value)
{
	if (Controller)
	{
		AddControllerYawInput(Value.Get<float>() * KeyboardTurnRateDeg * GetWorld()->GetDeltaSeconds());
	}
}

void AArenaCharacter::Input_Look(const FInputActionValue& Value)
{
	const FVector2D Delta = Value.Get<FVector2D>();
	const UArenaInputSetup* Setup = GetInputSetup();
	const float Sensitivity = Setup ? Setup->MouseSensitivity : 1.f;
	const float PitchDirection = (Setup && Setup->bInvertY) ? -1.f : 1.f;

	if (IsRightMouseDown())
	{
		AddControllerYawInput(Delta.X * Sensitivity);
		CameraPitch = FMath::Clamp(CameraPitch + Delta.Y * Sensitivity * PitchDirection, MinPitch, MaxPitch);
	}
	else if (IsLeftMouseDown())
	{
		CameraYawOffset = FMath::Fmod(CameraYawOffset + Delta.X * Sensitivity, 360.f);
		CameraPitch = FMath::Clamp(CameraPitch + Delta.Y * Sensitivity * PitchDirection, MinPitch, MaxPitch);
	}
}

void AArenaCharacter::Input_Zoom(const FInputActionValue& Value)
{
	if (SpringArm)
	{
		const UArenaInputSetup* Setup = GetInputSetup();
		const float Step = Setup ? Setup->ZoomStep : 50.f;
		SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength - Value.Get<float>() * Step, MinZoom, MaxZoom);
	}
}

void AArenaCharacter::Input_JumpStarted(const FInputActionValue& Value)
{
	Jump();
}

void AArenaCharacter::Input_JumpCompleted(const FInputActionValue& Value)
{
	StopJumping();
}

void AArenaCharacter::Input_CycleTarget(const FInputActionValue& Value)
{
	if (Targeting)
	{
		Targeting->CycleTarget();
	}
}

void AArenaCharacter::Input_Slot1(const FInputActionValue& Value) { ActivateSlot(1); }
void AArenaCharacter::Input_Slot2(const FInputActionValue& Value) { ActivateSlot(2); }
void AArenaCharacter::Input_Slot3(const FInputActionValue& Value) { ActivateSlot(3); }
void AArenaCharacter::Input_Slot4(const FInputActionValue& Value) { ActivateSlot(4); }
void AArenaCharacter::Input_Slot5(const FInputActionValue& Value) { ActivateSlot(5); }
void AArenaCharacter::Input_Slot6(const FInputActionValue& Value) { ActivateSlot(6); }
void AArenaCharacter::Input_Slot7(const FInputActionValue& Value) { ActivateSlot(7); }
void AArenaCharacter::Input_Slot8(const FInputActionValue& Value) { ActivateSlot(8); }

void AArenaCharacter::ActivateSlot(int32 Slot)
{
	if (AbilitySystem)
	{
		AbilitySystem->TryActivateSlot(Slot);
	}
}

UArenaInputSetup* AArenaCharacter::GetInputSetup() const
{
	const AArenaPlayerController* ArenaPC = Cast<AArenaPlayerController>(GetController());
	return ArenaPC ? const_cast<AArenaPlayerController*>(ArenaPC)->GetInputSetup() : nullptr;
}

bool AArenaCharacter::IsRightMouseDown() const
{
	const APlayerController* PC = Cast<APlayerController>(GetController());
	return PC && PC->IsInputKeyDown(EKeys::RightMouseButton);
}

bool AArenaCharacter::IsLeftMouseDown() const
{
	const APlayerController* PC = Cast<APlayerController>(GetController());
	return PC && PC->IsInputKeyDown(EKeys::LeftMouseButton);
}
