// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileWeapon.h"

#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	// 서버에서만 Projectile 발사
	if (!HasAuthority())
	{
		return;
	}
	
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash")))
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		const FRotator TargetRotation = ToTarget.Rotation();

		if (ProjectileClass && InstigatorPawn)
		{
			if (UWorld* World = GetWorld())
			{
				const FTransform ProjectileTransform(TargetRotation, SocketTransform.GetLocation());
				AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(ProjectileClass, ProjectileTransform, GetOwner(), InstigatorPawn);
				Projectile->SetDamage(Damage);
				Projectile->FinishSpawning(ProjectileTransform);
			}
		}
	}
}
