// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"

#include "Interface/LBCharacterWeaponInterface.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	ADSMultiplier = 0.5f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::SetAiming(bool bInAiming)
{
	if (!EquippedWeapon)
	{
		return;
	}
	
	bIsAiming = bInAiming;
	if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
	{
		Interface->SetADSWalkSpeed(bInAiming, ADSMultiplier);
	}
	
	ServerSetAiming(bInAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bInAiming)
{
	bIsAiming = bInAiming;
	if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
	{
		Interface->SetADSWalkSpeed(bInAiming, ADSMultiplier);
	}
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon)
	{
		if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
		{
			Interface->SetWeaponAnimLayers(EquippedWeapon->GetWeaponAnimLayer());
		}
	}
}

EWeaponType UCombatComponent::GetEquippingWeaponType() const
{
	if (EquippedWeapon)
	{
		return EquippedWeapon->GetWeaponType();
	}
	return EWeaponType();
}

void UCombatComponent::EquipWeapon(AWeapon* InWeapon)
{
	// From ServerRPC (Server Only)
	EquippedWeapon = InWeapon;

	if (EquippedWeapon)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
		{
			Interface->AttachWeapon(EquippedWeapon);
			Interface->SetWeaponAnimLayers(EquippedWeapon->GetWeaponAnimLayer());
		}
	}
}

