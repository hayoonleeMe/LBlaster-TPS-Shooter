// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CharacterOverlay.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCharacterOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	/* Weapon Slot Icon */
	WeaponSlotImages.Emplace(EEquipSlot::EES_FirstSlot, WeaponSlot1);
	WeaponSlotImages.Emplace(EEquipSlot::EES_SecondSlot, WeaponSlot2);
	WeaponSlotImages.Emplace(EEquipSlot::EES_ThirdSlot, WeaponSlot3);
}

void UCharacterOverlay::SetHealthBar(float InHealth, float InMaxHealth)
{
	const float HealthPercent = InHealth / InMaxHealth;
	HealthBar->SetPercent(HealthPercent);
}

void UCharacterOverlay::SetHealthText(float InHealth, float InMaxHealth)
{
	// TODO : 임시로 HealthText 표시 안함. 결정 필요 
	return;
	const FString HealthString = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(InHealth), FMath::CeilToInt(InMaxHealth));
	HealthText->SetText(FText::FromString(HealthString));
}

void UCharacterOverlay::SetAmmoAmount(int32 InAmmo)
{
	AmmoAmount->SetText(FText::AsNumber(InAmmo));
}

void UCharacterOverlay::SetCarriedAmmoAmount(int32 InCarriedAmmo)
{
	CarriedAmmoAmount->SetText(FText::AsNumber(InCarriedAmmo));
}

void UCharacterOverlay::SetWeaponTypeText(const FString& InWeaponTypeString)
{
	// TODO : 임시로 Weapon Type Text 사용안함. 완전히 결정 필요 
	return;
	
	if (InWeaponTypeString == FString())
	{
		WeaponTypeText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		WeaponTypeText->SetVisibility(ESlateVisibility::Visible);
		WeaponTypeText->SetText(FText::FromString(InWeaponTypeString));
	}
}

void UCharacterOverlay::SetGrenadeAmount(int32 InGrenadeAmount) const
{
	GrenadeAmount->SetText(FText::AsNumber(InGrenadeAmount));
}

void UCharacterOverlay::HighPingWarning(float InDuration)
{
	if (HighPingImage && HighPingImageBlink)
	{
		PlayAnimation(HighPingImageBlink, 0.f, FMath::CeilToInt32(InDuration / HighPingImageBlink->GetEndTime()));
	}
}

void UCharacterOverlay::SetWeaponSlotIcon(EEquipSlot InEquipSlot, EWeaponType InWeaponType)
{
	if (WeaponSlotIcons.Contains(InWeaponType) && WeaponSlotImages.Contains(InEquipSlot))
	{
		if (UTexture2D* WeaponTexture = WeaponSlotIcons[InWeaponType])
		{
			if (UImage* WeaponImage = WeaponSlotImages[InEquipSlot])
			{
				WeaponImage->SetBrushFromTexture(WeaponTexture, true);
			}
		}	
	}
}

void UCharacterOverlay::ChooseWeaponSlot(EEquipSlot InEquipSlot)
{
	for (const TTuple<EEquipSlot, UImage*>& Pair : WeaponSlotImages)
	{
		if (UImage* Image = Pair.Value)
		{
			if (Pair.Key == InEquipSlot)
			{
				Image->SetRenderOpacity(1.f);
			}
			else
			{
				Image->SetRenderOpacity(0.5f);
			}	
		}
	}
}
