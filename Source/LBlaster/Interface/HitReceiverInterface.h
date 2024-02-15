// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitReceiverInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LBLASTER_API IHitReceiverInterface
{
	GENERATED_BODY()

public:
	/*
	 *	ProjectileBullet <- LBlasterCharacter
	 */
	virtual void SetLastHitNormal(const FVector& InHitNormal) = 0;
	virtual AController* GetController() = 0; 
};
