// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Shotgun.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Interface/HitReceiverInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AShotgun::AShotgun()
{
	/* Weapon Type */
	WeaponType = EWeaponType::EWT_Shotgun;
	
	/* Ammo */
	NumberOfPellets = 12;

	/* Damage */
	Damage = 10;
}

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);
	
	if (UWorld* World = GetWorld(); const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName(TEXT("MuzzleFlash"))))
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector TraceStart = SocketTransform.GetLocation();

		TMap<IHitReceiverInterface*, FHitInfo> HitMap;
		for (uint32 Index = 0; Index < NumberOfPellets; ++Index)
		{
			const FVector TraceEnd = TraceStart + (HitTarget - TraceStart) * 1.25f;
			
			FHitResult FireHit;
			World->LineTraceSingleByChannel(FireHit, TraceStart, TraceEnd, ECC_Visibility);
			FVector BeamEnd = TraceEnd;

			if (FireHit.bBlockingHit && FireHit.GetActor())
			{
				BeamEnd = FireHit.ImpactPoint;
						
				// Caching Hit Info
				if (IHitReceiverInterface* HitInterface = Cast<IHitReceiverInterface>(FireHit.GetActor()))
				{
					if (HitMap.Contains(HitInterface))
					{
						++HitMap[HitInterface].HitCount;
					}
					else
					{
						HitMap.Emplace(HitInterface, { 1, FireHit.ImpactNormal, FireHit.GetActor() });
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

		for (const auto& HitPair : HitMap)
		{
			const FHitInfo& HitInfo = HitPair.Value; 
				
			// Play HitReact Montage
			HitPair.Key->SetLastHitNormal(HitInfo.ImpactNormal);
					
			// Apply Damage
			if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
			{
				if (AController* InstigatorController = OwnerPawn->Controller)
				{
					if (HasAuthority() && HitInfo.HitActor)
					{
						UGameplayStatics::ApplyDamage(HitInfo.HitActor, Damage * HitInfo.HitCount, InstigatorController, this, UDamageType::StaticClass());
					}	
				}
			}
		}
	}
}
