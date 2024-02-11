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
	virtual void Fire(const FVector& HitTarget) override;
	
private:
	/*
	 *	Projectile
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;
};
