// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"

#include "Interface/LBCharacterWeaponInterface.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::SetAiming(bool bInAiming)
{
	ServerSetAiming(bInAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bInAiming)
{
	bIsAiming = bInAiming;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::EquipWeapon(AWeapon* InWeapon)
{
	// From ServerRPC (Server Only)
	EquippedWeapon = InWeapon;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
	{
		Interface->AttachWeapon(EquippedWeapon);
	}
}

