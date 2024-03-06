// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;
	
private:
	/*
	 *	Impact
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Impact")
	TObjectPtr<UParticleSystem> ImpactParticle;
};
