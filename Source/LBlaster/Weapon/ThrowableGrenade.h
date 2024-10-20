// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/ProjectileGrenade.h"
#include "ThrowableGrenade.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AThrowableGrenade : public AProjectileGrenade
{
	GENERATED_BODY()

public:
	AThrowableGrenade();
	virtual void Tick(float DeltaSeconds) override;
	void SetInitialVelocity(const FVector& Velocity) const;
	
protected:
	virtual void BeginPlay() override;
	
private:
	// ThrowableGrenade Only
	UPROPERTY(EditAnywhere, Category="LBlaster|Explosive Damage", meta = (DisplayName = "Damage"))
	float DamageExposure;

	FVector RotationVelocity;
};
