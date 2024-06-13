// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/Pickup.h"

#include "Kismet/GameplayStatics.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	/* Auto Turn */
	BaseTurnRate = 45.f;
}
