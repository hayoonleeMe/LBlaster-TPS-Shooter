// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/ProjectileExplosive.h"
#include "ProjectileGrenade.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AProjectileGrenade : public AProjectileExplosive
{
	GENERATED_BODY()

public:
	AProjectileGrenade();
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:
	/*
	 *	Bounce
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Bounce")
	TObjectPtr<USoundBase> BounceSound;
};
