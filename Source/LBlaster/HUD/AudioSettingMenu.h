// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/BaseSettingMenu.h"
#include "AudioSettingMenu.generated.h"

USTRUCT()
struct FOriginalAudioGameUserSettings
{
	GENERATED_BODY()

	FOriginalAudioGameUserSettings()
		: OverallVolumeValue(0.f)
	{}

	UPROPERTY()
	float OverallVolumeValue;
};

/**
 * 
 */
UCLASS()
class LBLASTER_API UAudioSettingMenu : public UBaseSettingMenu
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;

protected:
	FOriginalAudioGameUserSettings OriginalSettings;

	/* Slider */
	float SliderMinValue = 0.f;
	float SliderMaxValue = 100.f;
	float SliderStepSize = 1.f;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USliderWithNumericInput> OverallVolumeSlider;

	void OnOverallVolumeSliderChanged(float InSliderValue);

	bool bChangedOverallVolume = false;
	
	/*
	 *	Apply Button
	 */
	virtual void OnApplyButtonClicked() override;
	virtual bool ShouldApplyChange() const override;

	/*
	 *	NoApplyAlertOverlay
	 */
	virtual void OnNoApplyAlertAcceptButtonClicked() override;
};
