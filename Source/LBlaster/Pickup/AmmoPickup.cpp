// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/AmmoPickup.h"

#include "Interface/LBCharacterPickupInterface.h"

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                  bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor)
	{
		if (ILBCharacterPickupInterface* Interface = Cast<ILBCharacterPickupInterface>(OtherActor))
		{
			Interface->PickupAmmo(WeaponType, AmmoAmount);
		}
	}

	Destroy();
}
