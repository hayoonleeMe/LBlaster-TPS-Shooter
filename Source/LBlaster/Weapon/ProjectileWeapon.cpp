// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileWeapon.h"

#include "LBlaster.h"
#include "Projectile.h"
#include "Character/LBlasterCharacter.h"
#include "Component/LagCompensationComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!IsValidOwnerCharacter() || !ProjectileClass)
	{
		return;
	}

	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash")))
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		const FRotator TargetRotation = ToTarget.Rotation();

		if (UWorld* World = GetWorld(); APawn* InstigatorPawn = Cast<APawn>(GetOwner()))
		{
			const FTransform ProjectileTransform(TargetRotation, SocketTransform.GetLocation());
			if (OwnerCharacter->HasAuthority())
			{
				// not-replicated
				AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(ProjectileClass, ProjectileTransform, GetOwner(), InstigatorPawn);
				Projectile->SetDamage(Damage, HeadshotMultiplier);
				Projectile->FinishSpawning(ProjectileTransform);
				Projectile->SetReplicatesPostInit(false);
			}
			else
			{
				// not-replicated
				AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(ProjectileClass, ProjectileTransform, GetOwner(), InstigatorPawn);
				Projectile->SetDamage(Damage, HeadshotMultiplier);
				Projectile->FinishSpawning(ProjectileTransform);
			}
		}
	}
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

			float DamageToCause = InDamage;
			if (Confirm.bHeadShot)
			{
				DamageToCause *= InHeadshotMultiplier;
			}
			// Apply Damage
			UGameplayStatics::ApplyDamage(HitCharacter, DamageToCause, OwnerCharacter->GetController(), this, UDamageType::StaticClass());
		}
	}
}
