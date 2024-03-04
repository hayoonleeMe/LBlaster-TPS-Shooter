// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LBlasterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void AnimNotify_ReloadFinished();

protected:
	bool IsValidCharacter();
	bool IsValidMovement();

private:
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> Character;

	UPROPERTY()
	TObjectPtr<class ULBCharacterMovementComponent> CharacterMovementComponent;
	
	/*
	 *	Character State
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Character State", meta = (AllowPrivateAccess = "true"))
	float GroundDistance;
	
	UPROPERTY(BlueprintReadOnly, Category="Character State", meta = (AllowPrivateAccess = "true"))
	uint8 bIsAiming : 1;
	
	UPROPERTY(BlueprintReadOnly, Category="Character State", meta = (AllowPrivateAccess = "true"))
	uint8 bIsFiring : 1;

	bool bPreserveAlign = false;

	/*
	 *	Left Hand
	 */
	UPROPERTY(BlueprintReadOnly, Category="Left Hand", meta = (AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly, Category="Left Hand", meta = (AllowPrivateAccess = "true"))
	uint8 bEnableFABRIK : 1;

	/*
	 *	Right Hand
	 */
	UPROPERTY(BlueprintReadOnly, Category="Right Hand", meta = (AllowPrivateAccess = "true"))
	FRotator RightHandRotation;

	UPROPERTY(BlueprintReadOnly, Category="Right Hand", meta = (AllowPrivateAccess = "true"))
	uint8 bEnableRightHandAlign : 1;

	void SetRightHandRotation();

	/*
	 *	Blend Weight
	 */
	UPROPERTY(BlueprintReadWrite, Category="Blend Weight", meta = (AllowPrivateAccess = "true"))
	float UpperbodyDynamicAdditiveWeight;
};
