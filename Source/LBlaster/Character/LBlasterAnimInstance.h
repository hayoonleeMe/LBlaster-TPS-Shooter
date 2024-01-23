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
	void UlBlasterAnim();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadonly, Category="Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ALBlasterCharacter> Owner;

	bool IsValidOwner();
	
	UPROPERTY(BlueprintReadonly, Category="Character", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;

	UPROPERTY(BlueprintReadonly, Category="Character", meta = (AllowPrivateAccess = "true"))
	uint8 bShouldMove : 1;
	
	UPROPERTY(BlueprintReadonly, Category="Character", meta = (AllowPrivateAccess = "true"))
	uint8 bIsFalling : 1;

	UPROPERTY(BlueprintReadonly, Category="Character", meta = (AllowPrivateAccess = "true"))
	uint8 bIsJumping : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category="Character", meta = (AllowPrivateAccess = "true"))
	float MovingThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category="Character", meta = (AllowPrivateAccess = "true"))
	float JumpingThreshold;
};
