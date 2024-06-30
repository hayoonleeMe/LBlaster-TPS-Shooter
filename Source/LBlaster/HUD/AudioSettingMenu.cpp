// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AudioSettingMenu.h"

#include "BaseHUD.h"
#include "SliderWithNumericInput.h"
#include "Components/Button.h"
#include "GameUserSettings/LBGameUserSettings.h"

void UAudioSettingMenu::MenuSetup()
{
	Super::MenuSetup();

	/* Slider */
	if (OverallVolumeSlider)
	{
		if (!OverallVolumeSlider->OnSliderValueChanged.IsBound())
		{
			OverallVolumeSlider->OnSliderValueChanged.BindUObject(this, &ThisClass::OnOverallVolumeSliderChanged);
		}

		OriginalSettings.OverallVolumeValue = GameUserSettings->GetOverallVolumeValue();
		OverallVolumeSlider->InitializeValues(GameUserSettings->GetOverallVolumeValue(), SliderMinValue, SliderMaxValue, SliderStepSize);	
	}
}

void UAudioSettingMenu::OnOverallVolumeSliderChanged(float InSliderValue)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetOverallVolumeValue(InSliderValue);

		bChangedOverallVolume = GameUserSettings->GetOverallVolumeValue() != OriginalSettings.OverallVolumeValue;
		EnableApplyButton();
	}
}

void UAudioSettingMenu::OnApplyButtonClicked()
{
	if (GameUserSettings)
	{
		GameUserSettings->ApplyCustomSettings(false, GetWorld());

		OriginalSettings.OverallVolumeValue = GameUserSettings->GetOverallVolumeValue();
		
		bChangedOverallVolume = false;
		
		if (ApplyButton)
		{
			ApplyButton->SetIsEnabled(false);
		}
	}
}

bool UAudioSettingMenu::ShouldApplyChange() const
{
	const bool bShouldApplyChange = bChangedOverallVolume;
	return bShouldApplyChange;
}

void UAudioSettingMenu::OnNoApplyAlertAcceptButtonClicked()
{
	// 변경사항 되돌리기
	if (GameUserSettings)
	{
		if (bChangedOverallVolume)
		{
			GameUserSettings->SetOverallVolumeValue(OriginalSettings.OverallVolumeValue);
		}
	}

	bChangedOverallVolume = false;
	
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu(true);
	}
}
