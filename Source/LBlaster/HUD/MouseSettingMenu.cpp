// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MouseSettingMenu.h"

#include "BaseHUD.h"
#include "SliderWithNumericInput.h"
#include "Components/Button.h"
#include "GameUserSettings/LBGameUserSettings.h"

void UMouseSettingMenu::MenuSetup()
{
	Super::MenuSetup();

	/* Slider */
	if (XAxisMouseSensitivitySlider)
	{
		if (!XAxisMouseSensitivitySlider->OnSliderValueChanged.IsBound())
		{
			XAxisMouseSensitivitySlider->OnSliderValueChanged.BindUObject(this, &ThisClass::OnXAxisMouseSensitivitySliderChanged);
		}

		OriginalSettings.XAxisMouseSensitivity = GameUserSettings->GetXAxisMouseSensitivity();
		XAxisMouseSensitivitySlider->InitializeValues(GameUserSettings->GetXAxisMouseSensitivity(), SliderMinValue, SliderMaxValue, SliderStepSize);
	}

	if (YAxisMouseSensitivitySlider)
	{
		if (!YAxisMouseSensitivitySlider->OnSliderValueChanged.IsBound())
		{
			YAxisMouseSensitivitySlider->OnSliderValueChanged.BindUObject(this, &ThisClass::OnYAxisMouseSensitivitySliderChanged);
		}

		OriginalSettings.YAxisMouseSensitivity = GameUserSettings->GetYAxisMouseSensitivity();
		YAxisMouseSensitivitySlider->InitializeValues(GameUserSettings->GetYAxisMouseSensitivity(), SliderMinValue, SliderMaxValue, SliderStepSize);
	}
}

void UMouseSettingMenu::OnXAxisMouseSensitivitySliderChanged(float InSliderValue)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetXAxisMouseSensitivity(InSliderValue);

		bChangedXAxisMouseSensitivity = GameUserSettings->GetXAxisMouseSensitivity() != OriginalSettings.XAxisMouseSensitivity;
		EnableApplyButton();
	}
}

void UMouseSettingMenu::OnYAxisMouseSensitivitySliderChanged(float InSliderValue)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetYAxisMouseSensitivity(InSliderValue);

		bChangedYAxisMouseSensitivity = GameUserSettings->GetYAxisMouseSensitivity() != OriginalSettings.YAxisMouseSensitivity;
		EnableApplyButton();
	}
}

void UMouseSettingMenu::OnApplyButtonClicked()
{
	if (GameUserSettings)
	{
		GameUserSettings->ApplyCustomSettings(false, GetWorld());

		OriginalSettings.XAxisMouseSensitivity = GameUserSettings->GetXAxisMouseSensitivity();
		OriginalSettings.YAxisMouseSensitivity = GameUserSettings->GetYAxisMouseSensitivity();
		
		bChangedXAxisMouseSensitivity = bChangedYAxisMouseSensitivity = false;
		
		if (ApplyButton)
		{
			ApplyButton->SetIsEnabled(false);
		}
	}
}

bool UMouseSettingMenu::ShouldApplyChange() const
{
	const bool bShouldApplyChange = bChangedXAxisMouseSensitivity || bChangedYAxisMouseSensitivity;
	return bShouldApplyChange;
}

void UMouseSettingMenu::OnNoApplyAlertAcceptButtonClicked()
{
	// 변경사항 되돌리기
	if (GameUserSettings)
	{
		if (bChangedXAxisMouseSensitivity)
		{
			GameUserSettings->SetXAxisMouseSensitivity(OriginalSettings.XAxisMouseSensitivity);
		}

		if (bChangedYAxisMouseSensitivity)
		{
			GameUserSettings->SetYAxisMouseSensitivity(OriginalSettings.YAxisMouseSensitivity);
		}
	}

	bChangedXAxisMouseSensitivity = bChangedYAxisMouseSensitivity = false;
	
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu(true);
	}
}
