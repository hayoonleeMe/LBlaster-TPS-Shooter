// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/ProjectileExplosive.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AProjectileRocket : public AProjectileExplosive
{
	GENERATED_BODY()

public:
	AProjectileRocket();
	virtual void Destroyed() override;
	virtual void SetReplicatesPostInit(bool bInReplicates) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
private:
	/*
	 *	Projectile Movement
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Projectile Movement")
	TObjectPtr<class URocketMovementComponent> RocketMovementComponent;
};
