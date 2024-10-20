// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileBullet.h"

#include "LBlaster.h"
#include "Weapon.h"
#include "Character/LBlasterCharacter.h"
#include "Component/LagCompensationComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LBlasterPlayerController.h"

AProjectileBullet::AProjectileBullet()
{
	/* Collision */
	CollisionBox->SetBoxExtent(FVector(5.f, 2.5f, 2.5f));

	/* Projectile Movement */
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	InitialSpeed = 15000.f;
	ProjectileMovementComponent->InitialSpeed = ProjectileMovementComponent->MaxSpeed = InitialSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.5f;
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValidOwnerCharacter() || !IsValidOwnerWeapon() || !OtherActor)
	{
		return;
	}

	if (ALBlasterCharacter* HitCharacter = Cast<ALBlasterCharacter>(OtherActor))
	{
		// 팀 데스매치에서 아군사격 방지 
		if (ALBlasterPlayerState* VictimState = HitCharacter->GetPlayerState<ALBlasterPlayerState>())
		{
			if (ALBlasterPlayerState* AttackerState = OwnerCharacter->GetPlayerState<ALBlasterPlayerState>())
			{
				if (VictimState->GetTeam() != ETeam::ET_MAX && VictimState->GetTeam() == AttackerState->GetTeam())
				{
					Destroy();
					return;
				}
			}
		}
		
		// Play HitReact Montage
		HitCharacter->SetLastHitNormal(Hit.ImpactNormal);
				
		// Apply Damage
		if (OwnerCharacter->HasAuthority() && (OwnerCharacter->IsLocallyControlled() || !OwnerCharacter->IsServerSideRewindEnabled()))
		{
			if (AController* InstigatorController = OwnerCharacter->GetController())
			{
				const float HitDistanceMeter = (Hit.ImpactPoint - TraceStart).Length() / 100.f;
				float DamageToCause = Damage * OwnerWeapon->GetDamageFallOffMultiplier(HitDistanceMeter);
				
				if (Hit.BoneName.ToString() == FString(TEXT("head")))
				{
					DamageToCause *= HeadshotMultiplier;
				}
				UGameplayStatics::ApplyDamage(HitCharacter, DamageToCause, InstigatorController, this, UDamageType::StaticClass());
			}
		}
		else if (!OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled() && OwnerCharacter->IsServerSideRewindEnabled())
		{
			// Apply Damage With Server-Side Rewind
			if (IsValidOwnerController())
			{
				const float HitTime = OwnerController->GetServerTime() - OwnerController->GetSingleTripTime();
				OwnerWeapon->CallServerScoreRequest(HitCharacter, TraceStart, InitialVelocity, HitTime, GetDamage(), HeadshotMultiplier, GetProjectileGravityScale());
			}
		}
	}
	
	Destroy();
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
	
	/* Lag Compensation */
	TraceStart = GetActorLocation();
	InitialVelocity = GetActorForwardVector() * InitialSpeed;
}
