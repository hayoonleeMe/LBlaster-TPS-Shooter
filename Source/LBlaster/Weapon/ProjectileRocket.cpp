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
	
	// ProjectileRocket은 클라이언트에서도 Hit Event가 발생하므로 데미지 로직은 서버에서만 수행
	if (HasAuthority())
	{
		if (OtherActor)
		{
			// Play HitReact Montage
			if (IHitReceiverInterface* HitInterface = Cast<IHitReceiverInterface>(OtherActor))
			{
				HitInterface->SetLastHitNormal(Hit.ImpactNormal);
			}
		}
	
		// Apply Damage
		if (const APawn* InstigatorPawn = GetInstigator())
		{
			if (AController* InstigatorController = InstigatorPawn->Controller)
			{
				UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, 10.f, GetActorLocation(), 200.f, 500.f, 1.f, UDamageType::StaticClass(), TArray<AActor*>(), this, InstigatorController);
			}
		}
	}

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
