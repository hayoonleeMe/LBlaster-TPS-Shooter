// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/VideoSettingMenu.h"

#include "LBlasterHUD.h"
#include "OptionSelector.h"
#include "SliderWithNumericInput.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "GameUserSettings/LBGameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void UVideoSettingMenu::MenuSetup()
{
	Super::MenuSetup();

	InitializeMenuOptions(true);
}

void UVideoSettingMenu::InitializeMenuOptions(bool bFirstCall)
{
	/* Slider */
	if (ScreenBrightnessSlider)
	{
		if (!ScreenBrightnessSlider->OnSliderValueChanged.IsBound())
		{
			ScreenBrightnessSlider->OnSliderValueChanged.BindUObject(this, &ThisClass::OnScreenBrightnessSliderChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.ScreenBrightnessValue = GameUserSettings->GetScreenBrightnessValue();
		}
		else
		{
			bChangedScreenBrightness = GameUserSettings->GetScreenBrightnessValue() != OriginalSettings.ScreenBrightnessValue;
		}
		ScreenBrightnessSlider->InitializeValues(GameUserSettings->GetScreenBrightnessValue(), SliderMinValue, SliderMaxValue, SliderStepSize);
	}
	
	/* Selector */
	if (DisplayModeSelector)
	{
		if (!DisplayModeSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			DisplayModeSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnDisplayModeChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.FullScreenMode = GameUserSettings->GetFullscreenMode();
		}
		else
		{
			bChangedFullScreenMode = GameUserSettings->GetFullscreenMode() != OriginalSettings.FullScreenMode;
		}
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

		if (bFirstCall)
		{
			OriginalSettings.ScreenResolution = GameUserSettings->GetScreenResolution();
		}
		else
		{
			bChangedScreenResolution = GameUserSettings->GetScreenResolution() != OriginalSettings.ScreenResolution;
		}
		for (int32 Index = 0; Index < ULBGameUserSettings::ScreenResolutionArray.Num(); ++Index)
		{
			if (ULBGameUserSettings::ScreenResolutionArray[Index] == GameUserSettings->GetScreenResolution())
			{
				ResolutionSelector->SetActiveIndex(Index);
				break;
			}
		}
	}
	
	if (PerformanceIndicatorSelector)
	{
		if (!PerformanceIndicatorSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			PerformanceIndicatorSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnPerformanceIndicatorChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.bEnablePerformanceIndicator = GameUserSettings->IsEnabledPerformanceIndicator();
		}
		else
		{
			bChangedEnablePerformanceIndicator = GameUserSettings->IsEnabledPerformanceIndicator() != OriginalSettings.bEnablePerformanceIndicator;
		}
		PerformanceIndicatorSelector->SetActiveIndex(GameUserSettings->IsEnabledPerformanceIndicator());
	}
	
	if (FPSLimitSelector)
	{
		if (!FPSLimitSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			FPSLimitSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnFPSLimitChanged);
		}

		const float FPSLimit = GameUserSettings->GetFrameRateLimit();
		if (bFirstCall)
		{
			OriginalSettings.FrameRateLimitValue = FPSLimit;
		}
		else
		{
			bChangedFrameLimitValue = GameUserSettings->GetFrameRateLimit() != OriginalSettings.FrameRateLimitValue;
		}
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

		if (bFirstCall)
		{
			OriginalSettings.bEnableVSync = GameUserSettings->IsVSyncEnabled();
		}
		else
		{
			bChangedEnableVSync = GameUserSettings->IsVSyncEnabled() != OriginalSettings.bEnableVSync;
		}
		VSyncSelector->SetActiveIndex(GameUserSettings->IsVSyncEnabled());
	}
	
	if (GraphicPresetSelector)
	{
		if (!GraphicPresetSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			GraphicPresetSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnGraphicPresetChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.GraphicPresetValue = GameUserSettings->GetGraphicPresetValue();
		}
		else
		{
			bChangedGraphicPresetValue = GameUserSettings->GetGraphicPresetValue() != OriginalSettings.GraphicPresetValue;
		}
		GraphicPresetSelector->SetActiveIndex(GameUserSettings->GetGraphicPresetValue());
	}

	if (AntiAliasingSelector)
	{
		if (!AntiAliasingSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			AntiAliasingSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnAntiAliasingChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.AntiAliasingValue = GameUserSettings->GetAntiAliasingQuality();
		}
		else
		{
			bChangedAntiAliasing = GameUserSettings->GetAntiAliasingQuality() != OriginalSettings.AntiAliasingValue;
		}
		AntiAliasingSelector->SetActiveIndex(GameUserSettings->GetAntiAliasingQuality());
	}
	
	if (ViewDistanceSelector)
	{
		if (!ViewDistanceSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			ViewDistanceSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnViewDistanceChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.ViewDistanceValue = GameUserSettings->GetViewDistanceQuality();
		}
		else
		{
			bChangedViewDistance = GameUserSettings->GetViewDistanceQuality() != OriginalSettings.ViewDistanceValue; 
		}
		ViewDistanceSelector->SetActiveIndex(GameUserSettings->GetViewDistanceQuality());
	}
	
	if (ShadowQualitySelector)
	{
		if (!ShadowQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			ShadowQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnShadowQualityChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.ShadowQualityValue = GameUserSettings->GetShadowQuality();
		}
		else
		{
			bChangedShadowQuality = GameUserSettings->GetShadowQuality() != OriginalSettings.ShadowQualityValue;
		}
		ShadowQualitySelector->SetActiveIndex(GameUserSettings->GetShadowQuality());
	}

	if (GlobalIlluminationQualitySelector)
	{
		if (!GlobalIlluminationQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			GlobalIlluminationQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnGlobalIlluminationQualityChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.GlobalIlluminationQualityValue = GameUserSettings->GetGlobalIlluminationQuality();
		}
		else
		{
			bChangedGlobalIlluminationQuality = GameUserSettings->GetGlobalIlluminationQuality() != OriginalSettings.GlobalIlluminationQualityValue;
		}
		GlobalIlluminationQualitySelector->SetActiveIndex(GameUserSettings->GetGlobalIlluminationQuality());
	}

	if (ReflectionQualitySelector)
	{
		if (!ReflectionQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			ReflectionQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnReflectionQualityChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.ReflectionQualityValue = GameUserSettings->GetReflectionQuality();
		}
		else
		{
			bChangedReflectionQuality = GameUserSettings->GetReflectionQuality() != OriginalSettings.ReflectionQualityValue;
		}
		ReflectionQualitySelector->SetActiveIndex(GameUserSettings->GetReflectionQuality());
	}
	
	if (PostProcessingSelector)
	{
		if (!PostProcessingSelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			PostProcessingSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnPostProcessingChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.PostProcessingValue = GameUserSettings->GetPostProcessingQuality();
		}
		else
		{
			bChangedPostProcessing = GameUserSettings->GetPostProcessingQuality() != OriginalSettings.PostProcessingValue;
		}
		PostProcessingSelector->SetActiveIndex(GameUserSettings->GetPostProcessingQuality());
	}
	
	if (TextureQualitySelector)
	{
		if (!TextureQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			TextureQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnTextureQualityChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.TextureQualityValue = GameUserSettings->GetTextureQuality();
		}
		else
		{
			bChangedTextureQuality = GameUserSettings->GetTextureQuality() != OriginalSettings.TextureQualityValue;
		}
		TextureQualitySelector->SetActiveIndex(GameUserSettings->GetTextureQuality());
	}
	
	if (EffectQualitySelector)
	{
		if (!EffectQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			EffectQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnEffectQualityChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.EffectQualityValue = GameUserSettings->GetVisualEffectQuality();
		}
		else
		{
			bChangedEffectQuality = GameUserSettings->GetVisualEffectQuality() != OriginalSettings.EffectQualityValue;
		}
		EffectQualitySelector->SetActiveIndex(GameUserSettings->GetVisualEffectQuality());
	}
	
	if (BackgroundQualitySelector)
	{
		if (!BackgroundQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			BackgroundQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnBackgroundQualityChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.BackgroundQualityValue = GameUserSettings->GetFoliageQuality();
		}
		else
		{
			bChangedBackgroundQuality = GameUserSettings->GetFoliageQuality() != OriginalSettings.BackgroundQualityValue;
		}
		BackgroundQualitySelector->SetActiveIndex(GameUserSettings->GetFoliageQuality());
	}
	
	if (ShadingQualitySelector)
	{
		if (!ShadingQualitySelector->OnSwitcherActiveIndexChanged.IsBound())
		{
			ShadingQualitySelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnShadingQualityChanged);
		}

		if (bFirstCall)
		{
			OriginalSettings.ShadingQualityValue = GameUserSettings->GetShadingQuality();
		}
		else
		{
			bChangedShadingQuality = GameUserSettings->GetShadingQuality() != OriginalSettings.ShadingQualityValue;
		}
		ShadingQualitySelector->SetActiveIndex(GameUserSettings->GetShadingQuality());
	}

	/* Graphic Quality Auto Set Button */
	if (GraphicQualityAutoSetButton && !GraphicQualityAutoSetButton->OnClicked.IsBound())
	{
		GraphicQualityAutoSetButton->OnClicked.AddDynamic(this, &ThisClass::OnGraphicQualityAutoSetButtonClicked);
	}
}

void UVideoSettingMenu::OnScreenBrightnessSliderChanged(float InSliderValue)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenBrightnessValue(InSliderValue);

		bChangedScreenBrightness = GameUserSettings->GetScreenBrightnessValue() != OriginalSettings.ScreenBrightnessValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnDisplayModeChanged(int32 InActiveIndex)
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

void UVideoSettingMenu::OnScreenResolutionChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenResolution(ULBGameUserSettings::ScreenResolutionArray[InActiveIndex]);
		
		bChangedScreenResolution = GameUserSettings->GetScreenResolution() != OriginalSettings.ScreenResolution;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnPerformanceIndicatorChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetPerformanceIndicatorEnabled(static_cast<bool>(InActiveIndex));

		bChangedEnablePerformanceIndicator = GameUserSettings->IsEnabledPerformanceIndicator() != OriginalSettings.bEnablePerformanceIndicator;
        EnableApplyButton();
	}
}

void UVideoSettingMenu::OnFPSLimitChanged(int32 InActiveIndex)
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

void UVideoSettingMenu::OnVSyncChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		// InActiveIndex : 0, 1 => false, true
		GameUserSettings->SetVSyncEnabled(static_cast<bool>(InActiveIndex));

		bChangedEnableVSync = GameUserSettings->IsVSyncEnabled() != OriginalSettings.bEnableVSync;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnGraphicPresetChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetGraphicPresetValue(InActiveIndex);

		bChangedGraphicPresetValue = GameUserSettings->GetGraphicPresetValue() != OriginalSettings.GraphicPresetValue;
		EnableApplyButton();
	}
	
	// 커스텀
	if (InActiveIndex == 4)
	{
		return;
	}
	
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

void UVideoSettingMenu::UpdateGraphicPresetSelector()
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
			GraphicPresetSelector->SetActiveIndex(GameUserSettings->GetAntiAliasingQuality());
			OnGraphicPresetChanged(GameUserSettings->GetAntiAliasingQuality());
		}
		// 커스텀
		else
		{
			GraphicPresetSelector->SetActiveIndex(4);
			OnGraphicPresetChanged(4);
		}
	}
}

void UVideoSettingMenu::OnAntiAliasingChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetAntiAliasingQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedAntiAliasing = GameUserSettings->GetAntiAliasingQuality() != OriginalSettings.AntiAliasingValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnViewDistanceChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetViewDistanceQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedViewDistance = GameUserSettings->GetViewDistanceQuality() != OriginalSettings.ViewDistanceValue; 
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnShadowQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetShadowQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedShadowQuality = GameUserSettings->GetShadowQuality() != OriginalSettings.ShadowQualityValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnGlobalIlluminationQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetGlobalIlluminationQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedGlobalIlluminationQuality = GameUserSettings->GetGlobalIlluminationQuality() != OriginalSettings.GlobalIlluminationQualityValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnReflectionQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetReflectionQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedReflectionQuality = GameUserSettings->GetReflectionQuality() != OriginalSettings.ReflectionQualityValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnPostProcessingChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetPostProcessingQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedPostProcessing = GameUserSettings->GetPostProcessingQuality() != OriginalSettings.PostProcessingValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnTextureQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetTextureQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedTextureQuality = GameUserSettings->GetTextureQuality() != OriginalSettings.TextureQualityValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnEffectQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetVisualEffectQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedEffectQuality = GameUserSettings->GetVisualEffectQuality() != OriginalSettings.EffectQualityValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnBackgroundQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetFoliageQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedBackgroundQuality = GameUserSettings->GetFoliageQuality() != OriginalSettings.BackgroundQualityValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnShadingQualityChanged(int32 InActiveIndex)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetShadingQuality(InActiveIndex);
		UpdateGraphicPresetSelector();

		bChangedShadingQuality = GameUserSettings->GetShadingQuality() != OriginalSettings.ShadingQualityValue;
		EnableApplyButton();
	}
}

void UVideoSettingMenu::OnGraphicQualityAutoSetButtonClicked()
{
	if (GameUserSettings)
	{
		GameUserSettings->SetGraphicOptionByAutoDetect();
	}
	InitializeMenuOptions(false);
	EnableApplyButton();
}

bool UVideoSettingMenu::ShouldApplyChange() const
{
	const bool bShouldApplyChange = bChangedFullScreenMode || bChangedScreenResolution || bChangedEnablePerformanceIndicator || bChangedFrameLimitValue || bChangedScreenBrightness || bChangedEnableVSync || bChangedGraphicPresetValue || bChangedAntiAliasing || bChangedViewDistance || bChangedShadowQuality || bChangedGlobalIlluminationQuality || bChangedReflectionQuality || bChangedPostProcessing || bChangedTextureQuality || bChangedEffectQuality || bChangedBackgroundQuality || bChangedShadingQuality;
	return bShouldApplyChange;
}

void UVideoSettingMenu::OnNoApplyAlertAcceptButtonClicked()
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

		if (bChangedEnablePerformanceIndicator)
		{
			GameUserSettings->SetPerformanceIndicatorEnabled(OriginalSettings.bEnablePerformanceIndicator);
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

	bChangedFullScreenMode = bChangedScreenResolution = bChangedEnablePerformanceIndicator = bChangedFrameLimitValue = bChangedScreenBrightness = bChangedEnableVSync = bChangedGraphicPresetValue = bChangedAntiAliasing = bChangedViewDistance = bChangedShadowQuality = bChangedGlobalIlluminationQuality = bChangedReflectionQuality = bChangedPostProcessing = bChangedTextureQuality = bChangedEffectQuality = bChangedBackgroundQuality = bChangedShadingQuality = false;
	
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu(true);
	}	
}

void UVideoSettingMenu::OnApplyButtonClicked()
{
	if (GameUserSettings)
	{
		GameUserSettings->ApplyCustomSettings(false, GetWorld());

		OriginalSettings.FullScreenMode = GameUserSettings->GetFullscreenMode();
		OriginalSettings.ScreenResolution = GameUserSettings->GetScreenResolution();
		OriginalSettings.bEnablePerformanceIndicator = GameUserSettings->IsEnabledPerformanceIndicator();
		OriginalSettings.FrameRateLimitValue = GameUserSettings->GetFrameRateLimit();
		OriginalSettings.ScreenBrightnessValue = GameUserSettings->GetScreenBrightnessValue();
		OriginalSettings.bEnableVSync = GameUserSettings->IsVSyncEnabled();
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
		
		bChangedFullScreenMode = bChangedScreenResolution = bChangedEnablePerformanceIndicator = bChangedFrameLimitValue = bChangedScreenBrightness = bChangedEnableVSync = bChangedGraphicPresetValue = bChangedAntiAliasing = bChangedViewDistance = bChangedShadowQuality = bChangedGlobalIlluminationQuality = bChangedReflectionQuality = bChangedPostProcessing = bChangedTextureQuality = bChangedEffectQuality = bChangedBackgroundQuality = bChangedShadingQuality = false;
		
		if (ApplyButton)
		{
			ApplyButton->SetIsEnabled(false);
		}
	}
}
