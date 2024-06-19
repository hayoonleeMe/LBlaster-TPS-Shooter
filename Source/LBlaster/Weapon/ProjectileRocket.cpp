// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileRocket.h"

#include "RocketMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	/* Collision */
	CollisionBox->SetBoxExtent(FVector(14.f, 3.f, 3.f));

	/* Projectile Movement */
	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("Rocket MovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;;
	RocketMovementComponent->SetIsReplicated(true);
	InitialSpeed = 2500.f;
	RocketMovementComponent->InitialSpeed = RocketMovementComponent->MaxSpeed = InitialSpeed;
	RocketMovementComponent->ProjectileGravityScale = 0.f;

	/* Destroy */
	DestroyTime = 15.f;
}

void AProjectileRocket::Destroyed()
{
	/* Explosive Damage */
	ExplodeDamage();

	// Impact Effect를 표시하기 위해 Projectile 액터를 제거하는 AProjectile::OnHit을 호출하는 대신 직접 수행
	SpawnImpactEffects();
	
	DeactivateTrailSystem();
}

void AProjectileRocket::SetReplicatesPostInit(bool bInReplicates)
{
	Super::SetReplicatesPostInit(bInReplicates);

	if (RocketMovementComponent)
	{
		RocketMovementComponent->SetIsReplicated(bInReplicates);
	}
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	SpawnTrailSystem();
	StartDestroyTimer();
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}
