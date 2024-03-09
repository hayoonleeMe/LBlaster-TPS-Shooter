// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileExplosive.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Components/AudioComponent.h"

AProjectileExplosive::AProjectileExplosive()
{
	/* Mesh */
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileExplosive::DestroyTimerFinished()
{
	Destroy();
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