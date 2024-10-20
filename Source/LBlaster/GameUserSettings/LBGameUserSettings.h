// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "LBGameUserSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPerformanceIndicatorEnabledChangedDelegate, bool /* bPerformanceIndicatorEnabled */);

/**
 * 
 */
UCLASS()
class LBLASTER_API ULBGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	FORCEINLINE bool IsFirstExecute() const { return bFirstExecute; }
	
	/*
	 *	Video Setting
	 */
	FORCEINLINE bool IsEnabledPerformanceIndicator() const { return bPerformanceIndicatorEnabled; }
	FORCEINLINE float GetScreenBrightnessValue() const { return ScreenBrightnessValue; }
	FORCEINLINE int32 GetGraphicPresetValue() const { return GraphicPresetValue; }

	void SetPerformanceIndicatorEnabled(bool bEnable);
	FORCEINLINE void SetScreenBrightnessValue(float InValue) { ScreenBrightnessValue = InValue; }
	FORCEINLINE void SetGraphicPresetValue(int32 InValue) { GraphicPresetValue = InValue; }

	void ApplyCustomSettings(bool bCheckForCommandLineOverrides, const UObject* WorldContextObject);
	void SetGraphicOptionByAutoDetect(bool bFirstExecute = false);

	// 화면 해상도 프리셋
	inline const static TArray<FIntPoint> ScreenResolutionArray{{ { 1280, 720 }, { 1280, 800 }, { 1366, 768 }, { 1600, 900 }, { 1920, 1080 } }};

	FOnPerformanceIndicatorEnabledChangedDelegate OnPerformanceIndicatorEnabledChanged;

	/*
	 *	Mouse Setting
	 */
	FORCEINLINE float GetXAxisMouseSensitivity() const { return XAxisMouseSensitivity; }
	FORCEINLINE float GetYAxisMouseSensitivity() const { return YAxisMouseSensitivity; }
	
	FORCEINLINE void SetXAxisMouseSensitivity(float InValue) { XAxisMouseSensitivity = InValue; }
	FORCEINLINE void SetYAxisMouseSensitivity(float InValue) { YAxisMouseSensitivity = InValue; }

	/*
	 *	Audio Setting
	 */
	FORCEINLINE float GetOverallVolumeValue() const { return OverallVolumeValue; }
	
	FORCEINLINE void SetOverallVolumeValue(float InValue) { OverallVolumeValue = InValue; }
	
private:
	UPROPERTY(config)
	bool bFirstExecute = true;
	
	// Video Setting
	/*
	 *	Performance Indicator
	 */
	UPROPERTY(config)
	bool bPerformanceIndicatorEnabled = false;

	/*
	 *	Screen Brightness
	 */
	UPROPERTY(config)
	float ScreenBrightnessValue = 50.f;

	/*
	 *	Graphic Preset
	 */
	UPROPERTY(config)
	int32 GraphicPresetValue = 0;

	// Mouse Setting
	/*
	 *	Mouse Sensitivity
	 */
	inline static const FVector2D MouseSensitivityInputRange{ 0.f, 100.f };
	inline static const FVector2D MouseSensitivityOutputRange{ 0.2f, 1.4f };
	
	// Scalar Modifier X Value, 0 ~ 100 => 0.2 ~ 1.4
	UPROPERTY(config)
	float XAxisMouseSensitivity = 50.f;

	// Scalar Modifier Y Value, 0 ~ 100 => 0.2 ~ 1.4
	UPROPERTY(config)
	float YAxisMouseSensitivity = 50.f;

	// Audio Setting
	/*
	 *	Overall Volume
	 */
	// 0 ~ 100 => 0.0 ~ 1.0
	UPROPERTY(config)
	float OverallVolumeValue = 100.f;
};
