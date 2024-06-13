// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPickup.h"

#include "LBlaster.h"
#include "Net/UnrealNetwork.h"

AWeaponPickup::AWeaponPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeaponPickup::OnWeaponEquipped(bool bInSelected)
{
	bShouldRotate = false;
	OnWeaponPickupEquipped.ExecuteIfBound(this);
	OnWeaponPickupEquipped.Unbind();
}

void AWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldRotate)
	{
		// WeaponPickup이 소환된 Spawn Point의 위치를 기준으로 Yaw 회전
		const FVector Offset = GetActorLocation() - ParentLocation;
		const FRotator Rotation(0.f, BaseTurnRate * DeltaTime, 0.f);
		const FVector NewLocation = ParentLocation + Rotation.RotateVector(Offset);
		const FRotator NewRotation = GetActorRotation() + Rotation;
		SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}

void AWeaponPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponPickup, bShouldRotate);
	DOREPLIFETIME(AWeaponPickup, ParentLocation);
}
