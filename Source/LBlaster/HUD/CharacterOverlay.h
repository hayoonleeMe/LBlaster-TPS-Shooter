// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LBTypes/EquipSlot.h"
#include "LBTypes/WeaponTypes.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBar(float InHealth, float InMaxHealth);
	void SetHealthText(float InHealth, float InMaxHealth);
	void SetScoreAmount(float InScore);
	void SetDeathAmount(int32 InDeath);
	void SetAmmoAmount(int32 InAmmo);
	void SetCarriedAmmoAmount(int32 InCarriedAmmo);
	void SetWeaponTypeText(const FString& InWeaponTypeString);
	void SetMatchCountdownText(float InCountdownTime);
	void SetGrenadeAmount(int32 InGrenadeAmount) const;
	void HighPingWarning(float InDuration);
	void SetWeaponSlotIcon(EEquipSlot InEquipSlot, EWeaponType InWeaponType);
	void ChooseWeaponSlot(EEquipSlot InEquipSlot);

protected:
	virtual void NativeConstruct() override;
	
private:
	/*
	 *	Widget
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> HealthText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DeathAmount;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponTypeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MatchCountdownText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GrenadeAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> HighPingImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> WeaponSlot1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> WeaponSlot2;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> WeaponSlot3;

	/*
	 *	Widget Animation
	 */
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> MatchCountdownBlink;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HighPingImageBlink;

	/*
	 *	Miscellaneous
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Miscellaneous")
	float ThresholdToPlayMatchCountdownBlinkAnim = 20.f;

	/*
	 *	Weapon Slot Icon
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Weapon Slot Icon")
	TMap<EWeaponType, class UTexture2D*> WeaponSlotIcons;

	UPROPERTY()
	TMap<EEquipSlot, class UImage*> WeaponSlotImages;
};
