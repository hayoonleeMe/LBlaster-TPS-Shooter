// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileRocket.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/HitReceiverInterface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Components/AudioComponent.h"

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

	/* Trail */
	TrailDestroyTime = 3.f;
}

void AProjectileRocket::Destroyed()
{
	
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
	
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem, RootComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
	}
	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(ProjectileLoop, RootComponent, FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, false, 1.f, 1.f, 0.f, LoopingSoundAttenuation, nullptr, false);
	}
}

void AProjectileRocket::TrailDestroyTimerFinished()
{
	Destroy();
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
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

	/* Trail */
	GetWorldTimerManager().SetTimer(TrailDestroyTimer, this, &ThisClass::TrailDestroyTimerFinished, TrailDestroyTime);

	// Impact Effect를 표시하기 위해 Projectile 액터를 제거하는 Super::OnHit을 호출하는 대신 직접 수행
	SpawnImpactEffects();
	
	if (RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController())
	{
		TrailSystemComponent->GetSystemInstanceController()->Deactivate();
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}
