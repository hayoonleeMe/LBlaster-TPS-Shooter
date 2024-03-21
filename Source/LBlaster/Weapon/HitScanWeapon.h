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
	AHitScanWeapon();
	virtual void Fire(const FVector& HitTarget) override;
	virtual FVector TraceEndWithScatter(const FVector& HitTarget) const override;
	virtual bool DoesUseScatter() const override { return bUseScatter; }
	
protected:
	/*
	 *	Impact
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Impact")
	TObjectPtr<UParticleSystem> ImpactParticle;

	UPROPERTY(EditAnywhere, Category="LBlaster|Impact")
	TObjectPtr<USoundBase> ImpactSound;

	void SpawnImpactEffects(UWorld* World, const FHitResult& HitResult);

	/*
	 *	Beam
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Beam")
	TObjectPtr<UParticleSystem> BeamParticle;

	/*
	 *	Trace End With Scatter
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon Scatter")
	float DistanceToSphere;

	// 이 SphereRadius로 정의되는 Sphere의 랜덤한 지점으로 총알이 발사된다. 이 값이 작을수록 맞추기 쉽다. bUseScatter 옵션을 사용해야 적용된다.
	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon Scatter")
	float SphereRadius;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon Scatter")
	uint8 bUseScatter : 1;
};
