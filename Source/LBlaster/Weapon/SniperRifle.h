// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HitScanWeapon.h"
#include "SniperRifle.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ASniperRifle : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	ASniperRifle();

	FORCEINLINE const TSubclassOf<UUserWidget>& GetSniperScopeClass() const { return SniperScopeClass; }
	
private:
	/*
	 *	Sniper Scope
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Sniper Scope")
	TSubclassOf<UUserWidget> SniperScopeClass; 
};
