// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileBullet.h"

#include "Interface/HitReceiverInterface.h"
#include "Kismet/GameplayStatics.h"

AProjectileBullet::AProjectileBullet()
{
	Damage = 20.f;
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		// Play HitReact Montage
		if (IHitReceiverInterface* HitInterface = Cast<IHitReceiverInterface>(OtherActor))
		{
			HitInterface->SetLastHitNormal(Hit.ImpactNormal);
		}
		// Apply Damage
		if (const APawn* InstigatorPawn = GetInstigator())
		{
			if (AController* InstigatorController = InstigatorPawn->Controller)
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, InstigatorController, this, UDamageType::StaticClass());
			}
		}
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
