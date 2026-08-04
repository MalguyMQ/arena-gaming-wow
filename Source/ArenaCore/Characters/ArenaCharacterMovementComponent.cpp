// Copyright Flagcat Studio. All Rights Reserved.

#include "Characters/ArenaCharacterMovementComponent.h"
#include "GameFramework/Character.h"

UArenaCharacterMovementComponent::UArenaCharacterMovementComponent()
{
	MaxWalkSpeed = 700.f;
	MaxAcceleration = 6000.f;
	BrakingDecelerationWalking = 6000.f;
	BrakingFrictionFactor = 2.f;
	GroundFriction = 8.f;
	JumpZVelocity = 520.f;
	GravityScale = 1.25f;
	AirControl = 0.35f;
	bOrientRotationToMovement = false;
	bUseControllerDesiredRotation = false;
}

float UArenaCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	if (CharacterOwner)
	{
		const FVector Accel2D = GetCurrentAcceleration().GetSafeNormal2D();
		const FVector Forward2D = CharacterOwner->GetActorForwardVector().GetSafeNormal2D();
		if (!Accel2D.IsNearlyZero() && FVector::DotProduct(Accel2D, Forward2D) < -0.5f)
		{
			MaxSpeed *= BackpedalSpeedMultiplier;
		}
	}

	return MaxSpeed;
}
