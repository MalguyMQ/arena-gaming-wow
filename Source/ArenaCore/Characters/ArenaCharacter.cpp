// Copyright Flagcat Studio. All Rights Reserved.

#include "Characters/ArenaCharacter.h"
#include "Characters/ArenaCharacterMovementComponent.h"
#include "Player/ArenaPlayerController.h"
#include "Player/ArenaInputSetup.h"
#include "ArenaCore.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AArenaCharacter::AArenaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UArenaCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

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
}

void AArenaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
