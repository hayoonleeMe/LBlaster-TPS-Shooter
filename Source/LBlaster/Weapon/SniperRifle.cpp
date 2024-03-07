// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SniperRifle.h"

ASniperRifle::ASniperRifle()
{
	/* Weapon Type */
	WeaponType = EWeaponType::EWT_SniperRifle;

	/* Damage */
	Damage = 100.f;

	/* ADS FOV */
	ADSFOV = 8.f;
}
