// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ThrowableGrenade.h"

#include "GameFramework/ProjectileMovementComponent.h"

AThrowableGrenade::AThrowableGrenade()
{
	/* Destroy */
	DestroyTime = 2.5f;

	/* Projectile Movement */
	ProjectileMovementComponent->Bounciness = 0.5f;
	ProjectileMovementComponent->Friction = 0.5f;

	/* Explosive Damage */
	DamageInnerRadius = 300.f;
	DamageOuterRadius = 600.f;
	
	/* ThrowableGrenade */
	DamageExposure = 120.f;
}

void AThrowableGrenade::SetInitialVelocity(const FVector& Velocity) const
{
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Velocity = Velocity;
	}
}

void AThrowableGrenade::BeginPlay()
{
	Super::BeginPlay();

	SetDamage(DamageExposure, 1.f);
}
