// Copyright Flagcat Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "ArenaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class UArenaInputSetup;
class UArenaAbilitySystemComponent;
class UArenaAttributeSet;
class UArenaTargetingComponent;

// (Victime, magnitude, type : 0 = dégâts, 1 = soins) — écouté par l'UI locale
// pour le combat text flottant. Diffusé par MulticastCombatFeedback.
DECLARE_MULTICAST_DELEGATE_ThreeParams(FArenaCombatFeedbackDelegate, AActor*, float, int32);

// Personnage d'arène avec contrôles caméra à la WoW :
// - clic droit maintenu : la souris oriente le personnage (mouselook)
// - clic gauche maintenu : la caméra orbite librement sans tourner le personnage
// - presser le clic droit réaligne le personnage sur la caméra (comportement WoW)
// Porte l'ASC (mode Full) et le set d'attributs ; la classe (kit + stats) est
// initialisée côté serveur depuis les CSV via InitializeClass.
UCLASS()
class ARENACORE_API AArenaCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	explicit AArenaCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UArenaAbilitySystemComponent* GetArenaAbilitySystemComponent() const { return AbilitySystem; }
	UArenaAttributeSet* GetArenaAttributeSet() const { return Attributes; }
	UArenaTargetingComponent* GetTargetingComponent() const { return Targeting; }

	// Identifiant de classe (ligne de StatTemplates.csv / Classes.csv).
	virtual FName GetClassId() const;
	virtual FText GetDisplayName() const;

	// Serveur : applique stats + kit de la classe depuis les CSV.
	void InitializeClass(FName InClassId);

	UFUNCTION(Server, Reliable)
	void ServerSetClass(FName InClassId);

	// Ligne d'Abilities.csv liée à un slot de barre d'action (NAME_None si vide).
	FName GetAbilityRowForSlot(int32 Slot) const;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastCombatFeedback(float Magnitude, int32 FeedbackType);

	static FArenaCombatFeedbackDelegate OnCombatFeedback;

protected:
	virtual void BeginPlay() override;

	void Input_MoveForward(const FInputActionValue& Value);
	void Input_Strafe(const FInputActionValue& Value);
	void Input_Turn(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Zoom(const FInputActionValue& Value);
	void Input_JumpStarted(const FInputActionValue& Value);
	void Input_JumpCompleted(const FInputActionValue& Value);
	void Input_CycleTarget(const FInputActionValue& Value);
	void Input_Slot1(const FInputActionValue& Value);
	void Input_Slot2(const FInputActionValue& Value);
	void Input_Slot3(const FInputActionValue& Value);
	void Input_Slot4(const FInputActionValue& Value);
	void Input_Slot5(const FInputActionValue& Value);
	void Input_Slot6(const FInputActionValue& Value);
	void Input_Slot7(const FInputActionValue& Value);
	void Input_Slot8(const FInputActionValue& Value);
	void ActivateSlot(int32 Slot);

	UPROPERTY(VisibleAnywhere, Category = "Arena|GAS")
	TObjectPtr<UArenaAbilitySystemComponent> AbilitySystem;

	UPROPERTY(VisibleAnywhere, Category = "Arena|GAS")
	TObjectPtr<UArenaAttributeSet> Attributes;

	UPROPERTY(VisibleAnywhere, Category = "Arena|Combat")
	TObjectPtr<UArenaTargetingComponent> Targeting;

	UPROPERTY(VisibleAnywhere, Category = "Arena|Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Arena|Camera")
	TObjectPtr<UCameraComponent> Camera;

	// Placeholder visuel (capsule + marqueur d'orientation) en attendant les vrais meshes (M4).
	UPROPERTY(VisibleAnywhere, Category = "Arena|Visual")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Arena|Visual")
	TObjectPtr<UStaticMeshComponent> FacingMarker;

	UPROPERTY()
	TObjectPtr<UWidgetComponent> NameplateComponent;

	// AbilitySlotRows[slot] = ligne d'Abilities.csv — répliqué pour la barre d'action cliente.
	UPROPERTY(Replicated)
	TArray<FName> AbilitySlotRows;

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
	void CreateNameplate();

	float CameraPitch = -15.f;
	float CameraYawOffset = 0.f;
	bool bMouselookHeld = false;
};
