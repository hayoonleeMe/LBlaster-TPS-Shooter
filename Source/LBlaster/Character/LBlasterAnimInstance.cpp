// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterAnimInstance.h"

#include "LBlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void ULBlasterAnimInstance::UlBlasterAnim()
{
	MovingThreshold = 3.f;
	JumpingThreshold = 100.f;
}

void ULBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	IsValidOwner();
}

void ULBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValidOwner())
	{
		return;
	}

	const FVector Velocity = Owner->GetCharacterMovement()->Velocity;
	const FVector Acceleration = Owner->GetCharacterMovement()->GetCurrentAcceleration();
	GroundSpeed = Velocity.Size2D();
	bShouldMove = GroundSpeed > MovingThreshold && Acceleration != FVector::ZeroVector;
	bIsFalling = Owner->GetCharacterMovement()->IsFalling();
	bIsJumping = bIsFallin g && Velocity.Z > JumpingThreshold;
}

bool ULBlasterAnimInstance::IsValidOwner()
{
	if (!Owner)
	{
		Owner = Cast<ALBlasterCharacter>(TryGetPawnOwner());
	}
	return Owner != nullptr;
}
