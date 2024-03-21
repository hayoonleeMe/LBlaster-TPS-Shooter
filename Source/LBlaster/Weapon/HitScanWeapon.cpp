// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Interface/HitReceiverInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"

AHitScanWeapon::AHitScanWeapon()
{
	/* Weapon Scatter */
	DistanceToSphere = 800.f;
	SphereRadius = 60.f;
	bUseScatter = false;
}

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (UWorld* World = GetWorld(); const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName(TEXT("MuzzleFlash"))))
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector TraceStart = SocketTransform.GetLocation();
		const FVector TraceEnd = TraceStart + (HitTarget - TraceStart) * 1.25f;
			
		FHitResult FireHit;
		World->LineTraceSingleByChannel(FireHit, TraceStart, TraceEnd, ECC_Visibility);
		FVector BeamEnd = TraceEnd;

		if (FireHit.bBlockingHit && FireHit.GetActor())
		{
			BeamEnd = FireHit.ImpactPoint;
						
			// Play HitReact Montage
			if (IHitReceiverInterface* HitInterface = Cast<IHitReceiverInterface>(FireHit.GetActor()))
			{
				HitInterface->SetLastHitNormal(FireHit.ImpactNormal);
			}
					
			// Apply Damage
			if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
			{
				if (AController* InstigatorController = OwnerPawn->Controller)
				{
					if (HasAuthority())
					{
						UGameplayStatics::ApplyDamage(FireHit.GetActor(), Damage, InstigatorController, this, UDamageType::StaticClass());
					}	
				}
			}

			// Impact Effect
			SpawnImpactEffects(World, FireHit);
		}

		// Beam Effect
		if (BeamParticle)
		{
			if (UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticle, SocketTransform))
			{
				Beam->SetVectorParameter(FName(TEXT("Target")), BeamEnd);
			}
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& HitTarget) const
{
	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName(TEXT("MuzzleFlash"))))
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector TraceStart = SocketTransform.GetLocation();

		const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
		const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - TraceStart;

		return TraceStart + ToEndLoc / ToEndLoc.Size() * TRACE_LENGTH;
	}
	return Super::TraceEndWithScatter(HitTarget);
}

void AHitScanWeapon::SpawnImpactEffects(UWorld* World, const FHitResult& HitResult)
{
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitResult.ImpactPoint);
	}
}
