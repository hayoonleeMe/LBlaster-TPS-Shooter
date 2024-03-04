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

	if (bIsFiring && !Character->IsFiring())
	{
		bPreserveAlign = true;
	}
	bIsFiring = Character->IsFiring();

	/* Left Hand */
	LeftHandTransform = Character->GetLeftHandTransform();
	bEnableFABRIK = Character->IsEquippingWeapon() && !Character->IsReloading();

	/* Right Hand */
	SetRightHandRotation();
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

void ULBlasterAnimInstance::SetRightHandRotation()
{
	const FRotator TargetRot = Character->GetRightHandRotation();
	RightHandRotation = FMath::Lerp(RightHandRotation, TargetRot, 0.2f);

	if (!Character->IsLocallyControlled() || !Character->IsEquippingWeapon())
	{
		bEnableRightHandAlign = false;
		return;
	}
	
	if (Character->IsReloading())
	{
		bPreserveAlign = false;
		bEnableRightHandAlign = false;
		return;
	}
	
	if (bIsAiming || bIsFiring)
	{
		bEnableRightHandAlign = true;
		return;
	}
	
	// 사격 멈추고 아주 약간의 시간 동안 Align 유지
	if (bPreserveAlign)
	{
		if (UpperbodyDynamicAdditiveWeight >= 0.9f)
		{
			bEnableRightHandAlign = true;
			return;
		}
		bPreserveAlign = false;
		bEnableRightHandAlign = false;
	}
	
	if (Character->GetCharacterMovement()->Velocity.Size() <= 0.f)
	{
		bEnableRightHandAlign = true;
	}
	else if (Character->bIsCrouched && Character->GetMovementVector().X == 1.f)
	{
		bEnableRightHandAlign = true;
	}
	else
	{
		bEnableRightHandAlign = false;
	}
}
