// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileRocket.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/HitReceiverInterface.h"
#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	/* Damage */
	Damage = 120.f;

	/* Collision */
	CollisionBox->SetBoxExtent(FVector(14.f, 3.f, 3.f));

	/* Projectile Movement */
	ProjectileMovementComponent->InitialSpeed = 2500.f;
	ProjectileMovementComponent->MaxSpeed = 2500.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	
	/* Mesh */
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
