// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LBCharacterWeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULBCharacterWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LBLASTER_API ILBCharacterWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Weapon -> Character
	virtual void SetOverlappingWeapon(class AWeapon* InWeapon) = 0;
	virtual void SetHUDAmmo(int32 InAmmo) = 0;
};
