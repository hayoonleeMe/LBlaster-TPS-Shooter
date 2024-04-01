// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class LBLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Destroyed() override;

	void SetDamage(float InDamage);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwnerCharacter;

	bool IsValidOwnerCharacter();

	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> OwnerController;

	bool IsValidOwnerController();

	/*
	 *	Collision
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	/*
	 *	Projectile Movement
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category="LBlaster|Projectile Movement");
	float InitialSpeed;

	/*
	 *	Tracer
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Tracer")
	TObjectPtr<UParticleSystem> Tracer;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystemComponent> TracerComponent;

	/*
	 *	Impact
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Impact")
	TObjectPtr<UParticleSystem> ImpactParticle;

	UPROPERTY(EditAnywhere, Category="LBlaster|Impact")
	TObjectPtr<USoundBase> ImpactSound;

	void SpawnImpactEffects();

	/*
	 *	Damage
	 */
	UPROPERTY(VisibleAnywhere)
	float Damage;
};
