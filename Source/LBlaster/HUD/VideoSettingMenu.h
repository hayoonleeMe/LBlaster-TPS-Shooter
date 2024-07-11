// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSettingMenu.h"
#include "VideoSettingMenu.generated.h"

USTRUCT()
struct FOriginalVideoGameUserSettings
{
	GENERATED_BODY()
	
	UPROPERTY()
	TEnumAsByte<EWindowMode::Type> FullScreenMode;

	UPROPERTY()
	FIntPoint ScreenResolution;

	UPROPERTY()
	bool bEnablePerformanceIndicator;
	
	UPROPERTY()
	float FrameRateLimitValue;
	
	UPROPERTY()
	float ScreenBrightnessValue;
	
	UPROPERTY()
	bool bEnableVSync;
	
	UPROPERTY()
	float GraphicPresetValue;
	
	UPROPERTY()
	float AntiAliasingValue;
	
	UPROPERTY()
	float ViewDistanceValue;
	
	UPROPERTY()
	float ShadowQualityValue;
	
	UPROPERTY()
	float GlobalIlluminationQualityValue;
	
	UPROPERTY()
	float ReflectionQualityValue;
	
	UPROPERTY()
	float PostProcessingValue;
	
	UPROPERTY()
	float TextureQualityValue;
	
	UPROPERTY()
	float EffectQualityValue;
	
	UPROPERTY()
	float BackgroundQualityValue;
	
	UPROPERTY()
	float ShadingQualityValue;
};

/**
 * 
 */
UCLASS()
class LBLASTER_API UVideoSettingMenu : public UBaseSettingMenu
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;
	void InitializeMenuOptions(bool bFirstCall);

protected:
	FOriginalVideoGameUserSettings OriginalSettings;

	/*
	 *	Slider
	 */
	float SliderMinValue = 0.f;
	float SliderMaxValue = 100.f;
	float SliderStepSize = 1.f;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USliderWithNumericInput> ScreenBrightnessSlider;

	void OnScreenBrightnessSliderChanged(float InSliderValue);

	bool bChangedScreenBrightness = false;

	/*
	 *	Selector
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UOptionSelector> DisplayModeSelector;

	bool bChangedFullScreenMode = false;

	void OnDisplayModeChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))	
	TObjectPtr<UOptionSelector> ResolutionSelector;

	bool bChangedScreenResolution = false;

	void OnScreenResolutionChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> PerformanceIndicatorSelector;

	bool bChangedEnablePerformanceIndicator = false;
	
	void OnPerformanceIndicatorChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> FPSLimitSelector;

	bool bChangedFrameLimitValue = false;

	void OnFPSLimitChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> VSyncSelector;

	bool bChangedEnableVSync = false;

	void OnVSyncChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> GraphicPresetSelector;

	bool bChangedGraphicPresetValue = false;
	
	void OnGraphicPresetChanged(int32 InActiveIndex);
	void UpdateGraphicPresetSelector();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> AntiAliasingSelector;

	bool bChangedAntiAliasing = false;

	void OnAntiAliasingChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> ViewDistanceSelector;

	bool bChangedViewDistance = false;
	
	void OnViewDistanceChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> ShadowQualitySelector;

	bool bChangedShadowQuality = false;

	void OnShadowQualityChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> GlobalIlluminationQualitySelector;
	
	bool bChangedGlobalIlluminationQuality = false;

	void OnGlobalIlluminationQualityChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> ReflectionQualitySelector;

	bool bChangedReflectionQuality = false;

	void OnReflectionQualityChanged(int32 InActiveIndex);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> PostProcessingSelector;

	bool bChangedPostProcessing = false;

	void OnPostProcessingChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> TextureQualitySelector;

	bool bChangedTextureQuality = false;
	
	void OnTextureQualityChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> EffectQualitySelector;

	bool bChangedEffectQuality = false;
	
	void OnEffectQualityChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> BackgroundQualitySelector;

	bool bChangedBackgroundQuality = false;
	
	void OnBackgroundQualityChanged(int32 InActiveIndex);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> ShadingQualitySelector;

	bool bChangedShadingQuality = false;
	
	void OnShadingQualityChanged(int32 InActiveIndex);

	/*
	 *	Graphic Quality Auto Set
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> GraphicQualityAutoSetButton;

	UFUNCTION()
	void OnGraphicQualityAutoSetButtonClicked();

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
