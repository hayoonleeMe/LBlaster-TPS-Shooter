// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "LBGameUserSettings.generated.h"

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
	FORCEINLINE float GetGraphicPresetValue() const { return GraphicPresetValue; }

	FORCEINLINE void SetFPSIndicatorEnabled(bool bEnable) { bFPSIndicatorEnabled = bEnable; }
	FORCEINLINE void SetScreenBrightnessValue(float InValue) { ScreenBrightnessValue = InValue; }
	FORCEINLINE void SetMotionBlurValue(float InValue) { MotionBlurValue = InValue; }
	FORCEINLINE void SetGraphicPresetValue(float InValue) { GraphicPresetValue = InValue; }

	void ApplyCustomSettings(bool bCheckForCommandLineOverrides, const UObject* WorldContextObject);

	virtual void SetToDefaults() override;
	
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
	float GraphicPresetValue = 0.f;
};
