// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Shotgun.h"

#include "LBlaster.h"
#include "Character/LBlasterCharacter.h"
#include "Component/LagCompensationComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/LBlasterPlayerController.h"

AShotgun::AShotgun()
{
	/* Weapon Type */
	WeaponType = EWeaponType::EWT_Shotgun;
	
	/* Ammo */
	NumberOfPellets = 12;

	/* Damage */
	Damage = 10;

	/* Scatter (Minute of Angle) */
	bUseScatter = false;

	/* Spread */
	SpreadRadius = 60.f;
}

void AShotgun::ShotgunFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const TArray<FVector_NetQuantize>& HitTargets)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}
	
	AWeapon::Fire(TraceStart, TraceRotation, HitTargets[0]);
	
	TMap<ALBlasterCharacter*, FHitInfo> HitMap;
	TMap<ALBlasterCharacter*, FHitInfo> HeadshotHitMap;
	for (const FVector& HitTarget : HitTargets)
	{
		const FVector TraceEnd = TraceStart + (HitTarget - TraceStart) * 1.25f;
		
		FHitResult FireHit;
		GetWorld()->LineTraceSingleByChannel(FireHit, TraceStart, TraceEnd, ECC_Visibility);
		FVector BeamEnd = TraceEnd;

		if (FireHit.bBlockingHit && FireHit.GetActor())
		{
			BeamEnd = FireHit.ImpactPoint;
					
			// Caching Hit Info
			if (ALBlasterCharacter* HitCharacter = Cast<ALBlasterCharacter>(FireHit.GetActor()))
			{
				// Play Hit React Montage
				HitCharacter->SetLastHitNormal(FireHit.ImpactNormal);
				
				const float HitDistanceMeter = (FireHit.ImpactPoint - TraceStart).Length() / 100.f;
				
				// Headshot
				if (FireHit.BoneName.ToString() == FString(TEXT("head")))
				{
					if (HeadshotHitMap.Contains(HitCharacter))
					{
						++HeadshotHitMap[HitCharacter].HitCount;
					}
					else
					{
						HeadshotHitMap.Emplace(HitCharacter, { 1, FireHit, HitDistanceMeter });
					}	
				}
				else
				{
					if (HitMap.Contains(HitCharacter))
					{
						++HitMap[HitCharacter].HitCount;
					}
					else
					{
						HitMap.Emplace(HitCharacter, { 1, FireHit, HitDistanceMeter });
					}	
				}
			}
		}

		// Beam Effect
		if (BeamParticle)
		{
			if (UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle, TraceStart, TraceRotation))
			{
				Beam->SetVectorParameter(FName(TEXT("Target")), BeamEnd);
			}
		}
	}

	// Apply Damage	
	if (HasAuthority() && (OwnerCharacter->IsLocallyControlled() || !OwnerCharacter->IsServerSideRewindEnabled()))
	{
		// Headshot
		for (const TTuple<ALBlasterCharacter*, FHitInfo>& HitPair : HeadshotHitMap)
		{
			if (ALBlasterCharacter* HitCharacter = HitPair.Key)
			{
				const FHitInfo& HitInfo = HitPair.Value;
				
				// 샷건의 유효 사거리를 넘으면 데미지는 0이므로 No Hit
				const float DamageToCause = Damage * GetDamageFallOffMultiplier(HitInfo.HitDistanceMeter);
				if (DamageToCause == 0.f)
				{
					continue;
				}

				// Impact Effect
				SpawnImpactEffects(GetWorld(), HitInfo.FireHit);

				// Apply Damage
				if (AController* InstigatorController = OwnerCharacter->GetController())
				{
					UGameplayStatics::ApplyDamage(HitCharacter, DamageToCause * HeadshotMultiplier * HitInfo.HitCount, InstigatorController, this, UDamageType::StaticClass());
				}
			}
		}
		
		for (const TTuple<ALBlasterCharacter*, FHitInfo>& HitPair : HitMap)
		{
			if (ALBlasterCharacter* HitCharacter = HitPair.Key)
			{
				const FHitInfo& HitInfo = HitPair.Value; 

				// 샷건의 유효 사거리를 넘으면 데미지는 0이므로 No Hit
				const float DamageToCause = Damage * GetDamageFallOffMultiplier(HitInfo.HitDistanceMeter);
				if (DamageToCause == 0.f)
				{
					continue;
				}

				// Impact Effect
				SpawnImpactEffects(GetWorld(), HitInfo.FireHit);

				// Apply Damage
				if (AController* InstigatorController = OwnerCharacter->GetController())
				{
					UGameplayStatics::ApplyDamage(HitCharacter, DamageToCause * HitInfo.HitCount, InstigatorController, this, UDamageType::StaticClass());
				}	
			}
		}	
	}
	else if (!HasAuthority() && OwnerCharacter->IsLocallyControlled() && OwnerCharacter->IsServerSideRewindEnabled())
	{
		TArray<ALBlasterCharacter*> HitCharacters;
		for (const TTuple<ALBlasterCharacter*, FHitInfo>& HitPair : HitMap)
		{
			if (ALBlasterCharacter* HitCharacter = HitPair.Key)
			{
				HitCharacters.Emplace(HitCharacter);
			}
		}

		// Apply Damage With Server-Side Rewind
		if (IsValidOwnerController())
		{
			const float HitTime = OwnerController->GetServerTime() - OwnerController->GetSingleTripTime();
			ShotgunServerScoreRequest(HitCharacters, TraceStart, HitTargets, HitTime, this);	
		}
	}
}

TArray<FVector_NetQuantize> AShotgun::ShotgunTraceEndWithScatter(const FVector_NetQuantize& TraceStart, const FVector& HitTarget) const
{
	TArray<FVector_NetQuantize> TraceHitTargets;
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
		
	for (uint32 Index = 0; Index < NumberOfPellets; ++Index)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SpreadRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - TraceStart;
		const FVector RandHitTarget = TraceStart + ToEndLoc / ToEndLoc.Size() * TRACE_LENGTH;  
		TraceHitTargets.Emplace(RandHitTarget);
	}
	return TraceHitTargets;
}

void AShotgun::ShotgunServerScoreRequest_Implementation(const TArray<ALBlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocations, float HitTime, AWeapon* DamageCauser)
{
	if (IsValidOwnerCharacter() && OwnerCharacter->GetLagCompensationComponent() && DamageCauser)
	{
		const FShotgunServerSideRewindResult Confirm = OwnerCharacter->GetLagCompensationComponent()->ShotgunServerSideRewind(HitCharacters, TraceStart, HitLocations, HitTime);

		for (ALBlasterCharacter* HitCharacter : HitCharacters)
		{
			float TotalDamage = 0.f;
			if (Confirm.HeadShots.Contains(HitCharacter))
			{
				const float HeadShotDamage = DamageCauser->GetDamage() * GetDamageFallOffMultiplier(Confirm.HeadShots[HitCharacter].HitDistanceMeter) * DamageCauser->GetHeadshotMultiplier() * Confirm.HeadShots[HitCharacter].HitCount;
				TotalDamage += HeadShotDamage;
			}
			if (Confirm.BodyShots.Contains(HitCharacter))
			{
				const float BodyShotDamage = DamageCauser->GetDamage() * GetDamageFallOffMultiplier(Confirm.BodyShots[HitCharacter].HitDistanceMeter) * Confirm.BodyShots[HitCharacter].HitCount;
				TotalDamage += BodyShotDamage;
			}

			if (TotalDamage != 0.f && OwnerCharacter->GetController())
			{
				UGameplayStatics::ApplyDamage(HitCharacter, TotalDamage, OwnerCharacter->GetController(), DamageCauser, UDamageType::StaticClass());
			}
		}
	}
}
