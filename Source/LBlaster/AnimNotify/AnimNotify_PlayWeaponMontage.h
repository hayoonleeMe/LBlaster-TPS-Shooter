// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlayWeaponMontage.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UAnimNotify_PlayWeaponMontage : public UAnimNotify
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool PlayWeaponMontage(USkeletalMeshComponent* MeshComp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AnimNotify)
	TObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AnimNotify)
	float RateScale = 1.f;
};
