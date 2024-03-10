// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileRocket.h"

#include "RocketMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Interface/HitReceiverInterface.h"
#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	/* Collision */
	CollisionBox->SetBoxExtent(FVector(14.f, 3.f, 3.f));

	/* Projectile Movement */
	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("Rocket MovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;;
	RocketMovementComponent->SetIsReplicated(true);
	RocketMovementComponent->InitialSpeed = 2500.f;
	RocketMovementComponent->MaxSpeed = 2500.f;
	RocketMovementComponent->ProjectileGravityScale = 0.f;

	/* Destroy */
	DestroyTime = 3.f;
}

void AProjectileRocket::Destroyed()
{
	// Super::Destroyed() 호출 방지
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	SpawnTrailSystem();
	
	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	/* Explosive Damage */
	ExplodeDamage();

	/* Destroy */
	StartDestroyTimer();

	// Impact Effect를 표시하기 위해 Projectile 액터를 제거하는 AProjectile::OnHit을 호출하는 대신 직접 수행
	SpawnImpactEffects();
	
	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	DeactivateTrailSystem();
}
