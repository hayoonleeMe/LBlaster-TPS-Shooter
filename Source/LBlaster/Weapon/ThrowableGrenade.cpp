// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ThrowableGrenade.h"

#include "GameFramework/ProjectileMovementComponent.h"

AThrowableGrenade::AThrowableGrenade()
{
	/* Destroy */
	DestroyTime = 2.5f;

	/* Projectile Movement */
	ProjectileMovementComponent->ProjectileGravityScale = 1.3f;
	ProjectileMovementComponent->Bounciness = 0.4f;
	ProjectileMovementComponent->Friction = 0.3f;

	/* ThrowableGrenade */
	DamageExposure = 120.f;
}

void AThrowableGrenade::BeginPlay()
{
	Super::BeginPlay();

	SetDamage(DamageExposure, 1.f);
}
