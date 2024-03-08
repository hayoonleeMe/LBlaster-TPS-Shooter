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

	if (!IsValidCharacter() || !IsValidMovement())
	{
		return;
	}

	/* Character State */
	bIsAiming = Character->IsAiming();
	const FLBlasterCharacterGroundInfo& GroundInfo = CharacterMovementComponent->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
	bIsFiring = Character->IsFiring();

	/* Left Hand */
	WeaponLeftHandTransform = Character->GetWeaponLeftHandTransform();
	bEnableModifyLeftHand = Character->IsEquippingWeapon();
}

void ULBlasterAnimInstance::AnimNotify_ReloadFinished()
{
	if (IsValidCharacter())
	{
		Character->ReloadFinished();
	}
}

bool ULBlasterAnimInstance::IsValidCharacter()
{
	if (!Character && TryGetPawnOwner())
	{
		Character = Cast<ALBlasterCharacter>(TryGetPawnOwner());
	}
	return Character != nullptr;
}

bool ULBlasterAnimInstance::IsValidMovement()
{
	if (!CharacterMovementComponent && IsValidCharacter() && Character->GetCharacterMovement())
	{
		CharacterMovementComponent = CastChecked<ULBCharacterMovementComponent>(Character->GetCharacterMovement());
	}
	return CharacterMovementComponent != nullptr;
}