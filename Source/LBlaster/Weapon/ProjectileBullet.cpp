// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileBullet.h"

#include "LBlaster.h"
#include "Character/LBlasterCharacter.h"
#include "Component/LagCompensationComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LBlasterPlayerController.h"
#include "Weapon/ProjectileWeapon.h"

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
	if (!IsValidOwnerCharacter() || !OtherActor)
	{
		return;
	}

	if (ALBlasterCharacter* HitCharacter = Cast<ALBlasterCharacter>(OtherActor))
	{
		if (OwnerCharacter == HitCharacter)
		{
			return;
		}
		
		// Play HitReact Montage
		HitCharacter->SetLastHitNormal(Hit.ImpactNormal);
				
		// Apply Damage
		if (OwnerCharacter->HasAuthority() && (OwnerCharacter->IsLocallyControlled() || !OwnerCharacter->IsServerSideRewindEnabled()))
		{
			if (AController* InstigatorController = OwnerCharacter->GetController())
			{
				UGameplayStatics::ApplyDamage(HitCharacter, Damage, InstigatorController, this, UDamageType::StaticClass());
			}
		}
		else if (!OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled() && OwnerCharacter->IsServerSideRewindEnabled())
		{
			// Apply Damage With Server-Side Rewind
			if (IsValidOwnerController() && OwnerCharacter->GetEquippingWeapon())
			{
				if (AProjectileWeapon* ProjectileWeapon = Cast<AProjectileWeapon>(OwnerCharacter->GetEquippingWeapon()))
				{
					const float HitTime = OwnerController->GetServerTime() - OwnerController->GetSingleTripTime();
					ProjectileWeapon->ServerScoreRequest(HitCharacter, TraceStart, InitialVelocity, HitTime, GetDamage(), GetProjectileGravityScale());	
				}
			}
		}
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	if (IsValidOwnerCharacter() && !OwnerCharacter->HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
	
	/* Lag Compensation */
	TraceStart = GetActorLocation();
	InitialVelocity = GetActorForwardVector() * InitialSpeed;
}
