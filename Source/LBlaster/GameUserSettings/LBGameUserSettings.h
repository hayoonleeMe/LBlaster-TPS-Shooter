// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "LBGameUserSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFPSIndicatorEnabledChangedDelegate, bool /* bFPSIndicatorEnabled */);

/**
 * 
 */
UCLASS()
class LBLASTER_API ULBGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	FORCEINLINE bool IsEnabledFPSIndicator() const { return bFPSIndicatorEnabled; }
	FORCEINLINE float GetScreenBrightnessValue() const { return ScreenBrightnessValue; }
	FORCEINLINE float GetMotionBlurValue() const { return MotionBlurValue; }
	FORCEINLINE int32 GetGraphicPresetValue() const { return GraphicPresetValue; }

	void SetFPSIndicatorEnabled(bool bEnable);
	FORCEINLINE void SetScreenBrightnessValue(float InValue) { ScreenBrightnessValue = InValue; }
	FORCEINLINE void SetMotionBlurValue(float InValue) { MotionBlurValue = InValue; }
	FORCEINLINE void SetGraphicPresetValue(int32 InValue) { GraphicPresetValue = InValue; }

	void ApplyCustomSettings(bool bCheckForCommandLineOverrides, const UObject* WorldContextObject);
	void SetGraphicOptionByAutoDetect(bool bFirstExecute = false);

	// 화면 해상도 프리셋
	inline const static TArray<FIntPoint> ScreenResolutionArray{{ { 1280, 720 }, { 1280, 800 }, { 1366, 768 }, { 1600, 900 }, { 1920, 1080 } }};

	FOnFPSIndicatorEnabledChangedDelegate OnFPSIndicatorEnabledChanged;
	
private:
	/*
	 *	FPS Indicator
	 */
	UPROPERTY(config)
	bool bFPSIndicatorEnabled = false;

	/*
	 *	Screen Brightness
	 */
	UPROPERTY(config)
	float ScreenBrightnessValue = 50.f;

	/*
	 *	Motion Blur
	 */
	UPROPERTY(config)
	float MotionBlurValue = 0.f;

	/*
	 *	Graphic Preset
	 */
	UPROPERTY(config)
	int32 GraphicPresetValue = 0;
};
