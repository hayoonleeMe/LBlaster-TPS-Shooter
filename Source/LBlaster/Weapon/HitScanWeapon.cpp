// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/HitScanWeapon.h"

#include "LBlaster.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/LBlasterCharacter.h"
#include "Component/LagCompensationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/LBlasterPlayerController.h"

AHitScanWeapon::AHitScanWeapon()
{
	/* Scatter (Minute of Angle) */
	MOA = 9.f;
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
						const float HitDistanceMeter = (FireHit.ImpactPoint - TraceStart).Length() / 100.f;
						float DamageToCause = Damage * GetDamageFallOffMultiplier(HitDistanceMeter);
						if (FireHit.BoneName.ToString() == FString(TEXT("head")))
						{
							DamageToCause *= HeadshotMultiplier;
						}
						UGameplayStatics::ApplyDamage(FireHit.GetActor(), DamageToCause, InstigatorController, this, UDamageType::StaticClass());
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
			float DamageToCause = DamageCauser->GetDamage() * GetDamageFallOffMultiplier(Confirm.HitDistanceMeter);
			if (Confirm.bHeadShot)
			{
				DamageToCause *= DamageCauser->GetHeadshotMultiplier();
			}
			// Apply Damage
			UGameplayStatics::ApplyDamage(HitCharacter, DamageToCause, OwnerCharacter->GetController(), DamageCauser, UDamageType::StaticClass());
		}
	}
}
