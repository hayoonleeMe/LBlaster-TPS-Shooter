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
	MinuteOfAngle = 9.f;
}

void AHitScanWeapon::Fire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector& HitTarget)
{
	if (!IsValidOwnerCharacter() || !GetWorld())
	{
		return;
	}
	
	Super::Fire(TraceStart, TraceRotation, HitTarget);

	const FVector TraceEnd = TraceStart + (HitTarget - TraceStart) * 1.25f;
		
	// 총과 소유 중인 캐릭터와의 LineTrace 방지 
	FCollisionQueryParams CollisionQueryParams;
	const TArray<const AActor*> IgnoredActors = { this, OwnerCharacter }; 
	CollisionQueryParams.AddIgnoredActors(IgnoredActors);
	
	FHitResult FireHit;
	GetWorld()->LineTraceSingleByChannel(FireHit, TraceStart, TraceEnd, ECC_Visibility, CollisionQueryParams);
	const FVector BeamEnd = FireHit.bBlockingHit ? FireHit.ImpactPoint : TraceEnd;
	
	// Beam Effect
	if (BeamParticle)
	{
		if (UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle, TraceStart, TraceRotation))
		{
			Beam->SetVectorParameter(FName(TEXT("Target")), BeamEnd);
		}
	}

	if (FireHit.bBlockingHit && FireHit.GetActor())
	{
		// Impact Effect
		SpawnImpactEffects(GetWorld(), FireHit);

		if (ALBlasterCharacter* HitCharacter = Cast<ALBlasterCharacter>(FireHit.GetActor()))
		{
			// 팀 데스매치에서 아군사격 방지
			if (ALBlasterPlayerState* VictimState = HitCharacter->GetPlayerState<ALBlasterPlayerState>())
			{
				if (ALBlasterPlayerState* AttackerState = OwnerCharacter->GetPlayerState<ALBlasterPlayerState>())
				{
					if (VictimState->GetTeam() != ETeam::ET_MAX && VictimState->GetTeam() == AttackerState->GetTeam())
					{
						return;
					}
				}
			}
			
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
