// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSettingMenu.h"
#include "MouseSettingMenu.generated.h"

USTRUCT()
struct FOriginalMouseGameUserSettings
{
	GENERATED_BODY()

	UPROPERTY()
	float XAxisMouseSensitivity;

	UPROPERTY()
	float YAxisMouseSensitivity;	
};

/**
 * 
 */
UCLASS()
class LBLASTER_API UMouseSettingMenu : public UBaseSettingMenu
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;

protected:
	FOriginalMouseGameUserSettings OriginalSettings;

	/* Slider */
	float SliderMinValue = 0.f;
	float SliderMaxValue = 100.f;
	float SliderStepSize = 1.f;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USliderWithNumericInput> XAxisMouseSensitivitySlider;

	void OnXAxisMouseSensitivitySliderChanged(float InSliderValue);

	bool bChangedXAxisMouseSensitivity = false;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderWithNumericInput> YAxisMouseSensitivitySlider;

	void OnYAxisMouseSensitivitySliderChanged(float InSliderValue);

	bool bChangedYAxisMouseSensitivity = false;

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
