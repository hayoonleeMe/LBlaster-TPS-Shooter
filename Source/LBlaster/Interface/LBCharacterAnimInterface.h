// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LBCharacterAnimInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULBCharacterAnimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LBLASTER_API ILBCharacterAnimInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool IsEquippedWeapon() = 0;
};
