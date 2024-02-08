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

	/*
	 *	Left Hand
	 */
	UPROPERTY(BlueprintReadOnly, Category="Left Hand", meta = (AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;
};
