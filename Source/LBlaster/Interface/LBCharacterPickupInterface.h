// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBTypes/WeaponTypes.h"
#include "UObject/Interface.h"
#include "LBCharacterPickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULBCharacterPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LBLASTER_API ILBCharacterPickupInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount) = 0;
	
	virtual void EquipNewOverlappingWeapon() = 0; 
};
