// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class LBLASTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();

protected:
	/*
	 *	Auto Turn
	 */	
	UPROPERTY(EditAnywhere, Category="LBlaster|Pickup|Auto Turn")
	float BaseTurnRate;
};
