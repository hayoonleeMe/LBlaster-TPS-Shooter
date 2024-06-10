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
	virtual void Fire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector& HitTarget) override;
	virtual bool DoesUseScatter() const override { return bUseScatter; }
	
protected:
	/*
	 *	Impact
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Impact")
	TObjectPtr<UParticleSystem> ImpactParticle;

	UPROPERTY(EditAnywhere, Category="LBlaster|Impact")
	TObjectPtr<USoundBase> ImpactSound;

	void SpawnImpactEffects(const UWorld* World, const FHitResult& HitResult) const;

	/*
	 *	Beam
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Beam")
	TObjectPtr<UParticleSystem> BeamParticle;

private:
	/*
	 *	Lag Compensation
	 */
	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(class ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, AWeapon* DamageCauser);
};
