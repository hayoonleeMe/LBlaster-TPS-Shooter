// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_PlayWeaponMontage.h"

#include "Character/LBlasterCharacter.h"
#include "Weapon/Weapon.h"

bool UAnimNotify_PlayWeaponMontage::PlayWeaponMontage(USkeletalMeshComponent* MeshComp)
{
	if (MontageToPlay && MeshComp && MeshComp->GetOwner())
	{
		if (ALBlasterCharacter* LBCharacter = Cast<ALBlasterCharacter>(MeshComp->GetOwner()))
		{
			if (AWeapon* EquippingWeapon = LBCharacter->GetEquippingWeapon())
			{
				USkeletalMeshComponent* EquippingWeaponMesh = EquippingWeapon->GetWeaponMesh();
				if (EquippingWeaponMesh && EquippingWeaponMesh->GetSkeletalMeshAsset())
				{
					if (USkeleton* EquippingWeaponSkeleton = EquippingWeaponMesh->GetSkeletalMeshAsset()->GetSkeleton())
					{
						if (USkeleton* MontageWeaponSkeleton = MontageToPlay->GetSkeleton())
						{
							if (EquippingWeaponSkeleton == MontageWeaponSkeleton)
							{
								if (UAnimInstance* WeaponAnimInstance = EquippingWeaponMesh->GetAnimInstance())
								{
									if (WeaponAnimInstance->Montage_Play(MontageToPlay, RateScale))
									{
										return true;
									}
								}
							}
						}
					}
				}
			}	
		}
	}
	return false;
}
