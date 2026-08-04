// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ArenaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UArenaInputSetup;

// Personnage d'arène avec contrôles caméra à la WoW :
// - clic droit maintenu : la souris oriente le personnage (mouselook)
// - clic gauche maintenu : la caméra orbite librement sans tourner le personnage
// - presser le clic droit réaligne le personnage sur la caméra (comportement WoW)
// M2+ : ASC/GAS, ciblage tab, attributs.
UCLASS()
class ARENACORE_API AArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	explicit AArenaCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	void Input_MoveForward(const FInputActionValue& Value);
	void Input_Strafe(const FInputActionValue& Value);
	void Input_Turn(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Zoom(const FInputActionValue& Value);
	void Input_JumpStarted(const FInputActionValue& Value);
	void Input_JumpCompleted(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, Category = "Arena|Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Arena|Camera")
	TObjectPtr<UCameraComponent> Camera;

	// Placeholder visuel (capsule + marqueur d'orientation) en attendant les vrais meshes (M4).
	UPROPERTY(VisibleAnywhere, Category = "Arena|Visual")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Arena|Visual")
	TObjectPtr<UStaticMeshComponent> FacingMarker;

	UPROPERTY(EditAnywhere, Category = "Arena|Camera")
	float KeyboardTurnRateDeg = 140.f;

	UPROPERTY(EditAnywhere, Category = "Arena|Camera")
	float MinZoom = 150.f;

	UPROPERTY(EditAnywhere, Category = "Arena|Camera")
	float MaxZoom = 900.f;

	UPROPERTY(EditAnywhere, Category = "Arena|Camera")
	float MinPitch = -80.f;

	UPROPERTY(EditAnywhere, Category = "Arena|Camera")
	float MaxPitch = 35.f;

private:
	UArenaInputSetup* GetInputSetup() const;
	bool IsRightMouseDown() const;
	bool IsLeftMouseDown() const;

	float CameraPitch = -15.f;
	float CameraYawOffset = 0.f;
	bool bMouselookHeld = false;
};
