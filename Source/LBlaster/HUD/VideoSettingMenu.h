// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "VideoSettingMenu.generated.h"

USTRUCT()
struct FOriginalGameUserSettings
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
	float XAxisMouseSensitivity;

	UPROPERTY()
	float YAxisMouseSensitivity;
	
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
class LBLASTER_API UVideoSettingMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;
	virtual bool IsOverlayOpened() override;
	virtual void CloseOverlay() override;
	void InitializeMenuOptions(bool bFirstCall);

protected:
	UPROPERTY()
	TObjectPtr<class ULBGameUserSettings> GameUserSettings;

	FOriginalGameUserSettings OriginalSettings;

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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderWithNumericInput> XAxisMouseSensitivitySlider;

	void OnXAxisMouseSensitivitySliderChanged(float InSliderValue);

	bool bChangedXAxisMouseSensitivity = false;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderWithNumericInput> YAxisMouseSensitivitySlider;

	void OnYAxisMouseSensitivitySliderChanged(float InSliderValue);

	bool bChangedYAxisMouseSensitivity = false;
	

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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ApplyButton;

	UFUNCTION()
	void OnApplyButtonClicked();
	
	void EnableApplyButton();
	bool ShouldApplyChange() const;

	/*
	 *	Return Button
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UFUNCTION()
	virtual void OnReturnButtonClicked();

	/*
	 *	NoApplyAlertOverlay
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UOverlay> NoApplyAlertOverlay;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NoApplyAlertAcceptButton;

	UFUNCTION()
	virtual void OnNoApplyAlertAcceptButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NoApplyAlertCancelButton;

	UFUNCTION()
	void OnNoApplyAlertCancelButtonClicked();
};
