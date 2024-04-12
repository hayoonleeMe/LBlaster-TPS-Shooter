// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HitScanWeapon.h"
#include "Shotgun.generated.h"

struct FHitInfo
{
	uint32 HitCount;
	FHitResult FireHit;
	float HitDistanceMeter;
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
	/*
	 *	Ammo
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Ammo")
	uint32 NumberOfPellets;

	/*
	 *	Spread
	 */
	UPROPERTY(EditAnywhere, Category="LBLaster|Spread")
	float SpreadRadius;

	/*
	 *	Lag Compensation
	 */
	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(const TArray<ALBlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime, AWeapon* DamageCauser);
};