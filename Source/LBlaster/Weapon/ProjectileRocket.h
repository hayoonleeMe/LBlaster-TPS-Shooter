// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileRocket();
	virtual void Destroyed() override;
	
protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;
	void TrailDestroyTimerFinished();

private:
	/*
	 *	Mesh
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> RocketMesh;

	/*
	 *	Trail
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Trail")
	TObjectPtr<class UNiagaraSystem> TrailSystem;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> TrailSystemComponent;

	UPROPERTY(EditAnywhere, Category="LBlaster|Trail")
	TObjectPtr<USoundBase> ProjectileLoop;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ProjectileLoopComponent; 
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Trail")
	TObjectPtr<USoundAttenuation> LoopingSoundAttenuation; 

	FTimerHandle TrailDestroyTimer;

	UPROPERTY(EditAnywhere, Category="LBlaster|Trail")
	float TrailDestroyTime;
};
