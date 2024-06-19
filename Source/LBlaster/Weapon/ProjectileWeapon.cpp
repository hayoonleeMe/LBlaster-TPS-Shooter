// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileWeapon.h"

#include "LBlaster.h"
#include "Projectile.h"
#include "Character/LBlasterCharacter.h"
#include "Component/LagCompensationComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

AProjectileWeapon::AProjectileWeapon()
{
	/* Scatter (Minute of Angle) */
	MOA = 2.f;
}

void AProjectileWeapon::Fire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector& HitTarget)
{
	Super::Fire(TraceStart, TraceRotation, HitTarget);
	
	if (!IsValidOwnerCharacter() || !ProjectileClass)
	{
		return;
	}

	const FVector ToTarget = HitTarget - TraceStart;
	const FRotator TargetRotation = ToTarget.Rotation();

	if (UWorld* World = GetWorld(); APawn* InstigatorPawn = Cast<APawn>(GetOwner()))
	{
		const FTransform ProjectileTransform(TargetRotation, TraceStart);
		if (OwnerCharacter->HasAuthority())
		{
			// not-replicated
			AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(ProjectileClass, ProjectileTransform, this, InstigatorPawn);
			Projectile->SetDamage(Damage, HeadshotMultiplier);
			Projectile->FinishSpawning(ProjectileTransform);
			Projectile->SetReplicatesPostInit(false);
		}
		else
		{
			// not-replicated
			AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(ProjectileClass, ProjectileTransform, this, InstigatorPawn);
			Projectile->SetDamage(Damage, HeadshotMultiplier);
			Projectile->FinishSpawning(ProjectileTransform);
		}
	}
}

void AProjectileWeapon::CallServerScoreRequest(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize100& InitialVelocity, float HitTime, float InDamage, float InHeadshotMultiplier, float InProjectileGravityScale)
{
	ServerScoreRequest(HitCharacter, TraceStart, InitialVelocity, HitTime, InDamage, InHeadshotMultiplier, InProjectileGravityScale);
}

void AProjectileWeapon::ServerScoreRequest_Implementation(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime, float InDamage, float InHeadshotMultiplier, float InProjectileGravityScale)
{
	if (IsValidOwnerCharacter() && OwnerCharacter->GetLagCompensationComponent() && HitCharacter && GetWorld())
	{
		const FServerSideRewindResult Confirm = OwnerCharacter->GetLagCompensationComponent()->ProjectileServerSideRewind(HitCharacter, TraceStart, InitialVelocity, HitTime, InProjectileGravityScale);
		if (Confirm.bHitConfirmed && OwnerCharacter->GetController())
		{
			// Play HitReact Montage
			HitCharacter->SetLastHitNormal(Confirm.ImpactNormal);

			float DamageToCause = InDamage * GetDamageFallOffMultiplier(Confirm.HitDistanceMeter);
			if (Confirm.bHeadShot)
			{
				DamageToCause *= InHeadshotMultiplier;
			}
			// Apply Damage
			UGameplayStatics::ApplyDamage(HitCharacter, DamageToCause, OwnerCharacter->GetController(), this, UDamageType::StaticClass());
		}
	}
}
