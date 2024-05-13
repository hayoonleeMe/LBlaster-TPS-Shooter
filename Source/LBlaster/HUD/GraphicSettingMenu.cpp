// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GraphicSettingMenu.h"

#include "LBlasterHUD.h"
#include "OptionSelector.h"
#include "SliderWithNumericInput.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "GameFramework/GameUserSettings.h"
#include "GameUserSettings/LBGameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void UGraphicSettingMenu::MenuSetup()
{
	Super::MenuSetup();

	if (!GameUserSettings && GEngine)
	{
		GameUserSettings = Cast<ULBGameUserSettings>(GEngine->GetGameUserSettings());
	}

	/* Slider */
	if (ScreenBrightnessSlider)
	{
		if (!ScreenBrightnessSlider->OnSliderValueChanged.IsBound())
		{
			ScreenBrightnessSlider->OnSliderValueChanged.BindUObject(this, &ThisClass::OnScreenBrightnessSliderChanged);
		}
		
		OriginalSettings.ScreenBrightnessValue = GameUserSettings->GetScreenBrightnessValue();
		ScreenBrightnessSlider->InitializeValues(GameUserSettings->GetScreenBrightnessValue(), SliderMinValue, SliderMaxValue, SliderStepSize);
	}
	
	/* Selector */
	if (DisplayModeSelector)
	{
		if (!DisplayModeSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			DisplayModeSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnDisplayModeChanged);
		}

		OriginalSettings.FullScreenMode = GameUserSettings->GetFullscreenMode();
		switch (GameUserSettings->GetFullscreenMode())
		{
		case EWindowMode::Windowed:
			DisplayModeSelector->SetActiveIndex(0);
			break;
		case EWindowMode::WindowedFullscreen:
			DisplayModeSelector->SetActiveIndex(1);
			break;
		case EWindowMode::Fullscreen:
			DisplayModeSelector->SetActiveIndex(2);
			break;
		}
	}
	
	if (ResolutionSelector)
	{
		if (!ResolutionSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			ResolutionSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnScreenResolutionChanged);
		}
		
		OriginalSettings.ScreenResolution = GameUserSettings->GetScreenResolution();
		for (int32 Index = 0; Index < ULBGameUserSettings::ScreenResolutionArray.Num(); ++Index)
		{
			if (ULBGameUserSettings::ScreenResolutionArray[Index] == GameUserSettings->GetScreenResolution())
			{
				ResolutionSelector->SetActiveIndex(Index);
				break;
			}
		}
	}
	
	if (FPSIndicatorSelector)
	{
		if (!FPSIndicatorSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			FPSIndicatorSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnFPSIndicatorChanged);
		}

		OriginalSettings.bEnableFPSIndicator = GameUserSettings->IsEnabledFPSIndicator();
		FPSIndicatorSelector->SetActiveIndex(GameUserSettings->IsEnabledFPSIndicator());
	}
	
	if (FPSLimitSelector)
	{
		if (!FPSLimitSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			FPSLimitSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnFPSLimitChanged);
		}

		const float FPSLimit = GameUserSettings->GetFrameRateLimit();
		OriginalSettings.FrameRateLimitValue = FPSLimit;
		if (FPSLimit == 60.f)
		{
			FPSLimitSelector->SetActiveIndex(0);
		}
		else if (FPSLimit == 120.f)
		{
			FPSLimitSelector->SetActiveIndex(1);
		}
		else if (FPSLimit == 0.f)
		{
			FPSLimitSelector->SetActiveIndex(2);
		}
	}
	
	if (VSyncSelector)
	{
		if (!VSyncSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			VSyncSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnVSyncChanged);
		}

		OriginalSettings.bEnableVSync = GameUserSettings->IsVSyncEnabled();
		VSyncSelector->SetActiveIndex(GameUserSettings->IsVSyncEnabled());
	}
	
	if (MotionBlurSelector)
	{
		if (!MotionBlurSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			MotionBlurSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnMotionBlurChanged);
		}

		OriginalSettings.MotionBlurValue = GameUserSettings->GetMotionBlurValue();
		if (GameUserSettings->GetMotionBlurValue() == 0.5f)
		{
			MotionBlurSelector->SetActiveIndex(1);
		}
		else if (GameUserSettings->GetMotionBlurValue() == 0.f)
		{
			MotionBlurSelector->SetActiveIndex(0);
		}
	}

	if (GraphicPresetSelector)
	{
		if (!GraphicPresetSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			GraphicPresetSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnGraphicPresetChanged);
		}

		OriginalSettings.GraphicPresetValue = GameUserSettings->GetGraphicPresetValue();
		GraphicPresetSelector->SetActiveIndex(GameUserSettings->GetGraphicPresetValue() - 1);
	}

	if (AntiAliasingSelector)
	{
		if (!AntiAliasingSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			AntiAliasingSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnAntiAliasingChanged);
		}

		OriginalSettings.AntiAliasingValue = GameUserSettings->GetAntiAliasingQuality();
		AntiAliasingSelector->SetActiveIndex(GameUserSettings->GetAntiAliasingQuality() - 1);
	}
	
	if (ViewDistanceSelector)
	{
		if (!ViewDistanceSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			ViewDistanceSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnViewDistanceChanged);
		}

		OriginalSettings.ViewDistanceValue = GameUserSettings->GetViewDistanceQuality();
		ViewDistanceSelector->SetActiveIndex(GameUserSettings->GetViewDistanceQuality() - 1);
	}
	
	if (ShadowQualitySelector)
	{
		if (!ShadowQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			ShadowQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnShadowQualityChanged);
		}

		OriginalSettings.ShadowQualityValue = GameUserSettings->GetShadowQuality();
		ShadowQualitySelector->SetActiveIndex(GameUserSettings->GetShadowQuality() - 1);
	}

	if (GlobalIlluminationQualitySelector)
	{
		if (!GlobalIlluminationQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			GlobalIlluminationQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnGlobalIlluminationQualityChanged);
		}

		OriginalSettings.GlobalIlluminationQualityValue = GameUserSettings->GetGlobalIlluminationQuality();
		GlobalIlluminationQualitySelector->SetActiveIndex(GameUserSettings->GetGlobalIlluminationQuality() - 1);
	}

	if (ReflectionQualitySelector)
	{
		if (!ReflectionQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			ReflectionQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnReflectionQualityChanged);
		}

		OriginalSettings.ReflectionQualityValue = GameUserSettings->GetReflectionQuality();
		ReflectionQualitySelector->SetActiveIndex(GameUserSettings->GetReflectionQuality() - 1);
	}
	
	if (PostProcessingSelector)
	{
		if (!PostProcessingSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			PostProcessingSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnPostProcessingChanged);
		}

		OriginalSettings.PostProcessingValue = GameUserSettings->GetPostProcessingQuality();
		PostProcessingSelector->SetActiveIndex(GameUserSettings->GetPostProcessingQuality() - 1);
	}
	
	if (TextureQualitySelector)
	{
		if (!TextureQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			TextureQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnTextureQualityChanged);
		}

		OriginalSettings.TextureQualityValue = GameUserSettings->GetTextureQuality();
		TextureQualitySelector->SetActiveIndex(GameUserSettings->GetTextureQuality() - 1);
	}
	
	if (EffectQualitySelector)
	{
		if (!EffectQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			EffectQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnEffectQualityChanged);
		}

		OriginalSettings.EffectQualityValue = GameUserSettings->GetVisualEffectQuality();
		EffectQualitySelector->SetActiveIndex(GameUserSettings->GetVisualEffectQuality() - 1);
	}
	
	if (BackgroundQualitySelector)
	{
		if (!BackgroundQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			BackgroundQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnBackgroundQualityChanged);
		}

		OriginalSettings.BackgroundQualityValue = GameUserSettings->GetFoliageQuality();
		BackgroundQualitySelector->SetActiveIndex(GameUserSettings->GetFoliageQuality() - 1);
	}
	
	if (ShadingQualitySelector)
	{
		if (!ShadingQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			ShadingQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnShadingQualityChanged);
		}

		OriginalSettings.ShadingQualityValue = GameUserSettings->GetShadingQuality();
		ShadingQualitySelector->SetActiveIndex(GameUserSettings->GetShadingQuality() - 1);
	}

	/* Apply Button */
	if (ApplyButton && !ApplyButton->OnClicked.IsBound())
	{
		ApplyButton->OnClicked.AddDynamic(this, &ThisClass::OnApplyButtonClicked);
	}
	ApplyButton->SetIsEnabled(false);

	/* Return Button */
	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}

	/* NoApplyAlertOverlay */
	if (NoApplyAlertOverlay)
	{
		NoApplyAlertOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (NoApplyAlertAcceptButton && !NoApplyAlertAcceptButton->OnClicked.IsBound())
	{
		NoApplyAlertAcceptButton->OnClicked.AddDynamic(this, &ThisClass::OnNoApplyAlertAcceptButtonClicked);
	}

	if (NoApplyAlertCancelButton && !NoApplyAlertCancelButton->OnClicked.IsBound())
	{
		NoApplyAlertCancelButton->OnClicked.AddDynamic(this, &ThisClass::OnNoApplyAlertCancelButtonClicked);
	}
}

bool UGraphicSettingMenu::IsOverlayOpened()
{
	if (NoApplyAlertOverlay && NoApplyAlertOverlay->IsVisible())
	{
		return true;
	}
	if (ShouldApplyChange())
	{
		return true;
	}
	return false;
}

void UGraphicSettingMenu::CloseOverlay()
{
	if (NoApplyAlertOverlay && NoApplyAlertOverlay->IsVisible())
	{
		OnNoApplyAlertCancelButtonClicked();
	}
	else
	{
		OnReturnButtonClicked();
	}
}

void UGraphicSettingMenu::OnScreenBrightnessSliderChanged(float InSliderValue)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenBrightnessValue(InSliderValue);

		bChangedScreenBrightness = GameUserSettings->GetScreenBrightnessValue() != OriginalSettings.ScreenBrightnessValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnDisplayModeChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		switch (InActiveIndex)
		{
		case 0:
			GameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
			break;
		case 1:
			GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
			break;
		case 2:
			GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
			break;
		}
		
		bChangedFullScreenMode = GameUserSettings->GetFullscreenMode() != OriginalSettings.FullScreenMode;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnScreenResolutionChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenResolution(ULBGameUserSettings::ScreenResolutionArray[InActiveIndex]);
		
		bChangedScreenResolution = GameUserSettings->GetScreenResolution() != OriginalSettings.ScreenResolution;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnFPSIndicatorChanged(int32 InActiveIndex)
{
	// TODO : CharacterOverlay에 별도의 UI 추가 및 연계
	if (GameUserSettings)
	{
		GameUserSettings->SetFPSIndicatorEnabled(static_cast<bool>(InActiveIndex));

		bChangedEnableFPSIndicator = GameUserSettings->IsEnabledFPSIndicator() != OriginalSettings.bEnableFPSIndicator;
        EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnFPSLimitChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		switch (InActiveIndex)
		{
		case 0:
			GameUserSettings->SetFrameRateLimit(60.f);
			break;
		case 1:
			GameUserSettings->SetFrameRateLimit(120.f);
			break;
		case 2:
			GameUserSettings->SetFrameRateLimit(0.f);
			break;
		}

		bChangedFrameLimitValue = GameUserSettings->GetFrameRateLimit() != OriginalSettings.FrameRateLimitValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnVSyncChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1 => false, true
		GameUserSettings->SetVSyncEnabled(static_cast<bool>(InActiveIndex));

		bChangedEnableVSync = GameUserSettings->IsVSyncEnabled() != OriginalSettings.bEnableVSync;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnMotionBlurChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1 => 0, 0.5
		GameUserSettings->SetMotionBlurValue(InActiveIndex * 0.5f);

		bChangedMotionBlur = GameUserSettings->GetMotionBlurValue() != OriginalSettings.MotionBlurValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnGraphicPresetChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetGraphicPresetValue(InActiveIndex + 1);

		bChangedGraphicPresetValue = GameUserSettings->GetGraphicPresetValue() != OriginalSettings.GraphicPresetValue;
		EnableApplyButton();
	}
	
	// 커스텀
	if (InActiveIndex == 4)
	{
		return;
	}
	
	// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetAntiAliasingQuality())
	{
		if (AntiAliasingSelector)
		{
			AntiAliasingSelector->SetActiveIndex(InActiveIndex);
		}
		OnAntiAliasingChanged(InActiveIndex);	
	}

	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetViewDistanceQuality())
	{
		if (ViewDistanceSelector)
		{
			ViewDistanceSelector->SetActiveIndex(InActiveIndex);
		}
		OnViewDistanceChanged(InActiveIndex);	
	}

	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetShadowQuality())
	{
		if (ShadowQualitySelector)
		{
			ShadowQualitySelector->SetActiveIndex(InActiveIndex);
		}
		OnShadowQualityChanged(InActiveIndex);
	}

	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetGlobalIlluminationQuality())
	{
		if (GlobalIlluminationQualitySelector)
		{
			GlobalIlluminationQualitySelector->SetActiveIndex(InActiveIndex);
		}
		OnGlobalIlluminationQualityChanged(InActiveIndex);	
	}

	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetReflectionQuality())
	{
		if (ReflectionQualitySelector)
		{
			ReflectionQualitySelector->SetActiveIndex(InActiveIndex);
		}
		OnReflectionQualityChanged(InActiveIndex);	
	}

	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetPostProcessingQuality())
	{
		if (PostProcessingSelector)
		{
			PostProcessingSelector->SetActiveIndex(InActiveIndex);
		}
		OnPostProcessingChanged(InActiveIndex);	
	}

	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetTextureQuality())
	{
		if (TextureQualitySelector)
		{
			TextureQualitySelector->SetActiveIndex(InActiveIndex);
		}
		OnTextureQualityChanged(InActiveIndex);	
	}

	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetVisualEffectQuality())
	{
		if (EffectQualitySelector)
    	{
    		EffectQualitySelector->SetActiveIndex(InActiveIndex);
    	}
    	OnEffectQualityChanged(InActiveIndex);	
	}

	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetFoliageQuality())
	{
		if (BackgroundQualitySelector)
    	{
    		BackgroundQualitySelector->SetActiveIndex(InActiveIndex);
    	}
    	OnBackgroundQualityChanged(InActiveIndex);	
	}

	if (GameUserSettings->GetGraphicPresetValue() != GameUserSettings->GetShadingQuality())
	{
		if (ShadingQualitySelector)
		{
			ShadingQualitySelector->SetActiveIndex(InActiveIndex);
		}
		OnShadingQualityChanged(InActiveIndex);	
	}
}

void UGraphicSettingMenu::UpdateGraphicPresetSelector()
{
	if (GameUserSettings && GraphicPresetSelector)
	{
		// 프리셋 사용
		if (GameUserSettings->GetAntiAliasingQuality() == GameUserSettings->GetViewDistanceQuality() &&
			GameUserSettings->GetAntiAliasingQuality() == GameUserSettings->GetShadowQuality() &&
			GameUserSettings->GetAntiAliasingQuality() == GameUserSettings->GetGlobalIlluminationQuality() &&
			GameUserSettings->GetAntiAliasingQuality() == GameUserSettings->GetReflectionQuality() &&
			GameUserSettings->GetAntiAliasingQuality() == GameUserSettings->GetPostProcessingQuality() &&
			GameUserSettings->GetAntiAliasingQuality() == GameUserSettings->GetTextureQuality() &&
			GameUserSettings->GetAntiAliasingQuality() == GameUserSettings->GetVisualEffectQuality() &&
			GameUserSettings->GetAntiAliasingQuality() == GameUserSettings->GetFoliageQuality() &&
			GameUserSettings->GetAntiAliasingQuality() == GameUserSettings->GetShadingQuality())
		{
			GraphicPresetSelector->SetActiveIndex(GameUserSettings->GetAntiAliasingQuality() - 1);
			OnGraphicPresetChanged(GameUserSettings->GetAntiAliasingQuality() - 1);
		}
		// 커스텀
		else
		{
			GraphicPresetSelector->SetActiveIndex(4);
			OnGraphicPresetChanged(4);
		}
	}
}

void UGraphicSettingMenu::OnAntiAliasingChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetAntiAliasingQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedAntiAliasing = GameUserSettings->GetAntiAliasingQuality() != OriginalSettings.AntiAliasingValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnViewDistanceChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetViewDistanceQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedViewDistance = GameUserSettings->GetViewDistanceQuality() != OriginalSettings.ViewDistanceValue; 
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnShadowQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetShadowQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedShadowQuality = GameUserSettings->GetShadowQuality() != OriginalSettings.ShadowQualityValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnGlobalIlluminationQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetGlobalIlluminationQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedGlobalIlluminationQuality = GameUserSettings->GetGlobalIlluminationQuality() != OriginalSettings.GlobalIlluminationQualityValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnReflectionQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetReflectionQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedReflectionQuality = GameUserSettings->GetReflectionQuality() != OriginalSettings.ReflectionQualityValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnPostProcessingChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetPostProcessingQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedPostProcessing = GameUserSettings->GetPostProcessingQuality() != OriginalSettings.PostProcessingValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnTextureQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetTextureQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedTextureQuality = GameUserSettings->GetTextureQuality() != OriginalSettings.TextureQualityValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnEffectQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetVisualEffectQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedEffectQuality = GameUserSettings->GetVisualEffectQuality() != OriginalSettings.EffectQualityValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnBackgroundQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetFoliageQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedBackgroundQuality = GameUserSettings->GetFoliageQuality() != OriginalSettings.BackgroundQualityValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::OnShadingQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1, 2, 3 => 1, 2, 3, 4
		GameUserSettings->SetShadingQuality(InActiveIndex + 1);
		UpdateGraphicPresetSelector();

		bChangedShadingQuality = GameUserSettings->GetShadingQuality() != OriginalSettings.ShadingQualityValue;
		EnableApplyButton();
	}
}

void UGraphicSettingMenu::EnableApplyButton()
{
	if (GameUserSettings && ApplyButton)
	{
		ApplyButton->SetIsEnabled(ShouldApplyChange());
	}
}

bool UGraphicSettingMenu::ShouldApplyChange() const
{
	const bool bShouldApplyChange = bChangedFullScreenMode || bChangedScreenResolution || bChangedEnableFPSIndicator || bChangedFrameLimitValue || bChangedScreenBrightness || bChangedEnableVSync || bChangedMotionBlur || bChangedGraphicPresetValue || bChangedAntiAliasing || bChangedViewDistance || bChangedShadowQuality || bChangedGlobalIlluminationQuality || bChangedReflectionQuality || bChangedPostProcessing || bChangedTextureQuality || bChangedEffectQuality || bChangedBackgroundQuality || bChangedShadingQuality;
	return bShouldApplyChange;
}

void UGraphicSettingMenu::OnReturnButtonClicked()
{
	if (ShouldApplyChange())
	{
		// 변경사항 있다고 alert
		if (NoApplyAlertOverlay)
		{
			NoApplyAlertOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (IsValidOwnerHUD())
		{
			OwnerHUD->ReturnMenu();
		}	
	}
}

void UGraphicSettingMenu::OnNoApplyAlertAcceptButtonClicked()
{
	// 변경사항 되돌리기
	if (GameUserSettings)
	{
		if (bChangedFullScreenMode)
		{
			GameUserSettings->SetFullscreenMode(OriginalSettings.FullScreenMode);
		}

		if (bChangedScreenResolution)
		{
			GameUserSettings->SetScreenResolution(OriginalSettings.ScreenResolution);
		}

		if (bChangedEnableFPSIndicator)
		{
			GameUserSettings->SetFPSIndicatorEnabled(OriginalSettings.bEnableFPSIndicator);
		}

		if (bChangedFrameLimitValue)
		{
			GameUserSettings->SetFrameRateLimit(OriginalSettings.FrameRateLimitValue);
		}

		if (bChangedScreenBrightness)
		{
			GameUserSettings->SetScreenBrightnessValue(OriginalSettings.ScreenBrightnessValue);
		}

		if (bChangedEnableVSync)
		{
			GameUserSettings->SetVSyncEnabled(OriginalSettings.bEnableVSync);
		}

		if (bChangedMotionBlur)
		{
			GameUserSettings->SetMotionBlurValue(OriginalSettings.MotionBlurValue);
		}

		if (bChangedGraphicPresetValue)
		{
			GameUserSettings->SetGraphicPresetValue(OriginalSettings.GraphicPresetValue);
		}

		if (bChangedAntiAliasing)
		{
			GameUserSettings->SetAntiAliasingQuality(OriginalSettings.AntiAliasingValue);
		}

		if (bChangedViewDistance)
		{
			GameUserSettings->SetViewDistanceQuality(OriginalSettings.ViewDistanceValue);
		}

		if (bChangedShadowQuality)
		{
			GameUserSettings->SetShadowQuality(OriginalSettings.ShadowQualityValue);
		}

		if (bChangedGlobalIlluminationQuality)
		{
			GameUserSettings->SetGlobalIlluminationQuality(OriginalSettings.GlobalIlluminationQualityValue);
		}

		if (bChangedReflectionQuality)
		{
			GameUserSettings->SetReflectionQuality(OriginalSettings.ReflectionQualityValue);
		}
		
		if (bChangedPostProcessing)
		{
			GameUserSettings->SetPostProcessingQuality(OriginalSettings.PostProcessingValue);
		}

		if (bChangedTextureQuality)
		{
			GameUserSettings->SetTextureQuality(OriginalSettings.TextureQualityValue);
		}

		if (bChangedEffectQuality)
		{
			GameUserSettings->SetVisualEffectQuality(OriginalSettings.EffectQualityValue);
		}

		if (bChangedBackgroundQuality)
        {
			GameUserSettings->SetFoliageQuality(OriginalSettings.BackgroundQualityValue);
        }	
        		
		if (bChangedShadingQuality)
		{
			GameUserSettings->SetShadingQuality(OriginalSettings.ShadingQualityValue);
		}	
	}

	bChangedFullScreenMode = bChangedScreenResolution = bChangedEnableFPSIndicator = bChangedFrameLimitValue = bChangedScreenBrightness = bChangedEnableVSync = bChangedMotionBlur = bChangedGraphicPresetValue = bChangedAntiAliasing = bChangedViewDistance = bChangedShadowQuality = bChangedGlobalIlluminationQuality = bChangedReflectionQuality = bChangedPostProcessing = bChangedTextureQuality = bChangedEffectQuality = bChangedBackgroundQuality = bChangedShadingQuality = false;
	
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu(true);
	}	
}

void UGraphicSettingMenu::OnNoApplyAlertCancelButtonClicked()
{
	if (NoApplyAlertOverlay)
	{
		NoApplyAlertOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGraphicSettingMenu::OnApplyButtonClicked()
{
	if (GameUserSettings)
	{
		GameUserSettings->ApplyCustomSettings(false, GetWorld());

		OriginalSettings.FullScreenMode = GameUserSettings->GetFullscreenMode();
		OriginalSettings.ScreenResolution = GameUserSettings->GetScreenResolution();
		OriginalSettings.bEnableFPSIndicator = GameUserSettings->IsEnabledFPSIndicator();
		OriginalSettings.FrameRateLimitValue = GameUserSettings->GetFrameRateLimit();
		OriginalSettings.ScreenBrightnessValue = GameUserSettings->GetScreenBrightnessValue();
		OriginalSettings.bEnableVSync = GameUserSettings->IsVSyncEnabled();
		OriginalSettings.MotionBlurValue = GameUserSettings->GetMotionBlurValue();
		OriginalSettings.GraphicPresetValue = GameUserSettings->GetGraphicPresetValue();
		OriginalSettings.AntiAliasingValue = GameUserSettings->GetAntiAliasingQuality();
		OriginalSettings.ViewDistanceValue = GameUserSettings->GetViewDistanceQuality();
		OriginalSettings.ShadowQualityValue = GameUserSettings->GetShadowQuality();
		OriginalSettings.GlobalIlluminationQualityValue = GameUserSettings->GetGlobalIlluminationQuality();
		OriginalSettings.ReflectionQualityValue = GameUserSettings->GetReflectionQuality();
		OriginalSettings.PostProcessingValue = GameUserSettings->GetPostProcessingQuality();
		OriginalSettings.TextureQualityValue = GameUserSettings->GetTextureQuality();
		OriginalSettings.EffectQualityValue = GameUserSettings->GetVisualEffectQuality();
		OriginalSettings.BackgroundQualityValue = GameUserSettings->GetFoliageQuality();
		OriginalSettings.ShadingQualityValue = GameUserSettings->GetShadingQuality();
		
		bChangedFullScreenMode = bChangedScreenResolution = bChangedEnableFPSIndicator = bChangedFrameLimitValue = bChangedScreenBrightness = bChangedEnableVSync = bChangedMotionBlur = bChangedGraphicPresetValue = bChangedAntiAliasing = bChangedViewDistance = bChangedShadowQuality = bChangedGlobalIlluminationQuality = bChangedReflectionQuality = bChangedPostProcessing = bChangedTextureQuality = bChangedEffectQuality = bChangedBackgroundQuality = bChangedShadingQuality = false;
		
		if (ApplyButton)
		{
			ApplyButton->SetIsEnabled(false);
		}
	}
}
