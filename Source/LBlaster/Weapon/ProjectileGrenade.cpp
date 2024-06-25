// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileGrenade.h"

#include "LBlaster.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectileGrenade::AProjectileGrenade()
{
	/* Collision Box */
	CollisionBox->SetBoxExtent(FVector(5.f, 2.f, 2.f));
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Ignore);
	
	/* Projectile Movement */
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile MovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->BounceVelocityStopSimulatingThreshold = 50.f;
	InitialSpeed = 2000.f;
	ProjectileMovementComponent->InitialSpeed = ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}

void AProjectileGrenade::Destroyed()
{
	ExplodeDamage();	
	Super::Destroyed();
}

void AProjectileGrenade::BeginPlay()
{
	// 상위 클래스인 AProjectile::BeginPlay()는 필요 없으므로 AActor::BeginPlay() 호출
	AActor::BeginPlay();

	SpawnTrailSystem();
	StartDestroyTimer();

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &ThisClass::OnBounce);
}

void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
	}
}
