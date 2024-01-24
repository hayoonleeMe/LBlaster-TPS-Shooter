// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"

#include "Interface/LBCharacterWeaponInterface.h"
#include "Weapon/Weapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

