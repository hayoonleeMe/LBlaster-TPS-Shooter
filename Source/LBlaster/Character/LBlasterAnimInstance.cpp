// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterAnimInstance.h"
#include "LBlasterCharacter.h"
#include "Component/LBCharacterMovementComponent.h"

void ULBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void ULBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValidCharacter())
	{
		return;
	}
	if (!IsValidMovement())
	{
		return;
	}

	/* Character State */
	bIsAiming = Character->IsAiming();
	const FLBlasterCharacterGroundInfo& GroundInfo = CharacterMovementComponent->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;

	bIsFiring = Character->IsFiring();

	/* Left Hand */
	LeftHandTransform = Character->GetLeftHandTransform();
}

bool ULBlasterAnimInstance::IsValidCharacter()
{
	if (!Character)
	{
		Character = Cast<ALBlasterCharacter>(TryGetPawnOwner());
	}
	return Character != nullptr;
}

bool ULBlasterAnimInstance::IsValidMovement()
{
	if (!CharacterMovementComponent)
	{
		CharacterMovementComponent = CastChecked<ULBCharacterMovementComponent>(Character->GetCharacterMovement());
	}
	return CharacterMovementComponent != nullptr;
}
