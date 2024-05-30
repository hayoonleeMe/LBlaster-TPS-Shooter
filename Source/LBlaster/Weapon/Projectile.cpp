// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Projectile.h"

#include "LBlaster.h"
#include "Weapon.h"
#include "Character/LBlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LBlasterPlayerController.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	/* Collision Box */
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);
}

void AProjectile::SetReplicatesPostInit(bool bInReplicates)
{
	SetReplicates(bInReplicates);
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->SetIsReplicated(bInReplicates);
	}
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(Tracer, CollisionBox, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}

	CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

bool AProjectile::IsValidOwnerCharacter()
{
	if (!OwnerCharacter && IsValidOwnerWeapon() && OwnerWeapon->GetOwner())
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(OwnerWeapon->GetOwner());
	}
	return OwnerCharacter != nullptr;
}

bool AProjectile::IsValidOwnerWeapon()
{
	if (!OwnerWeapon && GetOwner())
	{
		OwnerWeapon = Cast<AWeapon>(GetOwner());
	}
	return OwnerWeapon != nullptr;
}

bool AProjectile::IsValidOwnerController()
{
	if (!OwnerController && OwnerCharacter && OwnerCharacter->GetController())
	{
		OwnerController = Cast<ALBlasterPlayerController>(OwnerCharacter->GetController());
	}
	return OwnerController != nullptr;
}

void AProjectile::SpawnImpactEffects()
{
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

void AProjectile::Destroyed()
{
	SpawnImpactEffects();
	
	Super::Destroyed();
}

void AProjectile::SetDamage(float InDamage, float InHeadshotMultiplier)
{
	Damage = InDamage;
	HeadshotMultiplier = InHeadshotMultiplier;
}

float AProjectile::GetProjectileGravityScale() const
{
	if (ProjectileMovementComponent)
	{
		return ProjectileMovementComponent->ProjectileGravityScale;
	}
	return 0.f;
}

