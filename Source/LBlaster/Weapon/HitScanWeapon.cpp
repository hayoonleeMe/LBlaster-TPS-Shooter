// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/HitScanWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Character/LBlasterCharacter.h"
#include "Component/LagCompensationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/LBlasterPlayerController.h"

AHitScanWeapon::AHitScanWeapon()
{
	/* Weapon Scatter */
	DistanceToSphere = 800.f;
	SphereRadius = 60.f;
	bUseScatter = false;
}

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}
	
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

			// Impact Effect
			SpawnImpactEffects(World, FireHit);
			
			if (ALBlasterCharacter* HitCharacter = Cast<ALBlasterCharacter>(FireHit.GetActor()))
			{
				// Play HitReact Montage
				HitCharacter->SetLastHitNormal(FireHit.ImpactNormal);
				
				// Apply Damage
				if (HasAuthority() && (OwnerCharacter->IsLocallyControlled() || !OwnerCharacter->IsServerSideRewindEnabled()))
				{
					if (AController* InstigatorController = OwnerCharacter->GetController())
					{
						UGameplayStatics::ApplyDamage(FireHit.GetActor(), Damage, InstigatorController, this, UDamageType::StaticClass());
					}
				}
				else if (!HasAuthority() && OwnerCharacter->IsLocallyControlled() && OwnerCharacter->IsServerSideRewindEnabled())
				{
					// Apply Damage With Server-Side Rewind
					if (IsValidOwnerController())
					{
						const float HitTime = OwnerController->GetServerTime() - OwnerController->GetSingleTripTime();
						ServerScoreRequest(HitCharacter, TraceStart, HitTarget, HitTime, this);	
					}
				}
			}
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

void AHitScanWeapon::SpawnImpactEffects(const UWorld* World, const FHitResult& HitResult) const
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

void AHitScanWeapon::ServerScoreRequest_Implementation(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize& HitLocation, float HitTime, AWeapon* DamageCauser)
{
	if (IsValidOwnerCharacter() && OwnerCharacter->GetLagCompensationComponent() && HitCharacter && GetWorld())
	{
		const FServerSideRewindResult Confirm = OwnerCharacter->GetLagCompensationComponent()->ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);
		if (Confirm.bHitConfirmed && DamageCauser && OwnerCharacter->GetController())
		{
			// Apply Damage
			UGameplayStatics::ApplyDamage(HitCharacter, DamageCauser->GetDamage(), OwnerCharacter->GetController(), DamageCauser, UDamageType::StaticClass());
		}
	}
}
