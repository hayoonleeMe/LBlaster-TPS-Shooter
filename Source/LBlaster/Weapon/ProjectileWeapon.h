// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AProjectileWeapon();
	virtual void Fire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector& HitTarget) override;

	virtual void CallServerScoreRequest(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime, float InDamage, float InHeadshotMultiplier, float InProjectileGravityScale) override;
	
private:
	/*
	 *	Projectile
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;

	/*
	 *	Lag Compensation
	 */
	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(class ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime, float InDamage, float InHeadshotMultiplier, float InProjectileGravityScale);
};
