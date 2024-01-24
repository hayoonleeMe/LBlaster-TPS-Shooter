// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/LBCharacterAnimInterface.h"

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
	bIsJumping = bIsFalling && Velocity.Z > JumpingThreshold;
	bIsCrouched = Owner->bIsCrouched;
	
	// TODO : Tick에서 매번 인터페이스로 캐스팅하는 것이 성능에 괜찮은지 체크
	if (ILBCharacterAnimInterface* Interface = Cast<ILBCharacterAnimInterface>(Owner))
	{
		bIsEquippedWeapon = Interface->IsEquippedWeapon();
	}
}

bool ULBlasterAnimInstance::IsValidOwner()
{
	if (!Owner)
	{
		Owner = Cast<ACharacter>(TryGetPawnOwner());
	}
	return Owner != nullptr;
}