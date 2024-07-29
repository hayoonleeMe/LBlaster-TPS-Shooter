// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ThrowableGrenade.h"

#include "GameFramework/ProjectileMovementComponent.h"

AThrowableGrenade::AThrowableGrenade()
{
	PrimaryActorTick.bCanEverTick = true;
	
	/* Destroy */
	DestroyTime = 2.5f;

	/* Projectile Movement */
	ProjectileMovementComponent->Bounciness = 0.5f;
	ProjectileMovementComponent->Friction = 0.5f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;

	/* Explosive Damage */
	DamageInnerRadius = 300.f;
	DamageOuterRadius = 600.f;
	
	/* ThrowableGrenade */
	DamageExposure = 120.f;

	// 임의의 회전 속도 설정
	RotationVelocity = FVector(FMath::FRandRange(-360.0f, 360.0f), FMath::FRandRange(-360.0f, 360.0f), FMath::FRandRange(-360.0f, 360.0f));
}

void AThrowableGrenade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 회전 적용
	if (ProjectileMesh && ProjectileMovementComponent)
	{
		const FQuat QuatRotation = FQuat(RotationVelocity.Rotation() * 0.1f);
		ProjectileMesh->AddLocalRotation(QuatRotation);
	}
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
