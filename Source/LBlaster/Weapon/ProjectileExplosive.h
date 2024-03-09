// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Projectile.h"
#include "ProjectileExplosive.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AProjectileExplosive : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileExplosive();	
	
protected:
	/*
	 *	Mesh
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	/*
	 *	Trail
	 */
	void SpawnTrailSystem();
	void DeactivateTrailSystem() const;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Trail")
	TObjectPtr<class UNiagaraSystem> TrailSystem;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> TrailSystemComponent;

	UPROPERTY(EditAnywhere, Category="LBlaster|Trail")
	TObjectPtr<USoundBase> ProjectileLoopSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ProjectileLoopAudioComponent; 
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Trail")
	TObjectPtr<USoundAttenuation> LoopingSoundAttenuation; 

	/*
	 *	Destroy
	 */
	void StartDestroyTimer();
	void DestroyTimerFinished();
	
	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere, Category="LBlaster|Trail")
	float DestroyTime;
};
