// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileExplosive.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Components/AudioComponent.h"
#include "DamageType/DamageType_Explosive.h"

AProjectileExplosive::AProjectileExplosive()
{
	/* Mesh */
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Explosive Damage */
	DamageInnerRadius = 200.f;
	DamageOuterRadius = 500.f;
}

void AProjectileExplosive::DestroyTimerFinished()
{
	Destroy();
}

void AProjectileExplosive::ExplodeDamage()
{
	// 데미지 로직은 서버에서만 수행
	if (HasAuthority())
	{
		// Apply Damage
		if (const APawn* InstigatorPawn = GetInstigator())
		{
			if (AController* InstigatorController = InstigatorPawn->Controller)
			{
				// UDamageType_Explosive 타입을 사용해 폭발 데미지임을 알림.
				// 이후 Character에서 데미지를 받을 때 UDamageType_Explosive이면 랜덤한 HitReact Montage를 재생
				UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, 10.f, GetActorLocation(), DamageInnerRadius, DamageOuterRadius, 1.f, UDamageType_Explosive::StaticClass(), TArray<AActor*>(), this, InstigatorController);
			}
		}
	}
}

void AProjectileExplosive::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem, RootComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
	}
	if (ProjectileLoopSound && LoopingSoundAttenuation)
	{
		ProjectileLoopAudioComponent = UGameplayStatics::SpawnSoundAttached(ProjectileLoopSound, RootComponent, FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, false, 1.f, 1.f, 0.f, LoopingSoundAttenuation, nullptr, false);
	}
}

void AProjectileExplosive::DeactivateTrailSystem() const
{
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController())
	{
		TrailSystemComponent->GetSystemInstanceController()->Deactivate();
	}
	if (ProjectileLoopAudioComponent && ProjectileLoopAudioComponent->IsPlaying())
	{
		ProjectileLoopAudioComponent->Stop();
	}
}

void AProjectileExplosive::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ThisClass::DestroyTimerFinished, DestroyTime);
}