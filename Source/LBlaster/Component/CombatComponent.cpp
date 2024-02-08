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

	DOREPLIFETIME(UCombatComponent, EquippingWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::SetAiming(bool bInAiming)
{
	if (!EquippingWeapon)
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
	if (EquippingWeapon)
	{
		if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
		{
			Interface->SetWeaponAnimLayers(EquippingWeapon->GetWeaponAnimLayer());
		}
	}
}

EWeaponType UCombatComponent::GetEquippingWeaponType() const
{
	if (EquippingWeapon)
	{
		return EquippingWeapon->GetWeaponType();
	}
	return EWeaponType();
}

void UCombatComponent::EquipWeapon(AWeapon* InWeapon)
{
	// From ServerRPC (Server Only)
	EquippingWeapon = InWeapon;

	if (EquippingWeapon)
	{
		EquippingWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
		{
			Interface->AttachWeapon(EquippingWeapon);
			Interface->SetWeaponAnimLayers(EquippingWeapon->GetWeaponAnimLayer());
		}
	}
}

