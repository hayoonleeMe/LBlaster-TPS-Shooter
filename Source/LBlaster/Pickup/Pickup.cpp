// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/Pickup.h"

#include "Kismet/GameplayStatics.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	/* Auto Turn */
	BaseTurnRate = 45.f;

	// 모든 Actor 클래스에 상속되는 기본 USceneComponent
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}
