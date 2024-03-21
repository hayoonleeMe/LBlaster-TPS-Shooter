// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HitScanWeapon.h"
#include "Shotgun.generated.h"

struct FHitInfo
{
	uint32 HitCount;
	FVector ImpactNormal;
	AActor* HitActor;
};

/**
 * 
 */
UCLASS()
class LBLASTER_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	AShotgun();
	virtual void ShotgunFire(const TArray<FVector_NetQuantize>& HitTargets) override;
	virtual TArray<FVector_NetQuantize> ShotgunTraceEndWithScatter(const FVector& HitTarget) const override;
	
private:
	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	uint32 NumberOfPellets;
};