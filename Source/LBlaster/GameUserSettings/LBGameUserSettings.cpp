// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUserSettings/LBGameUserSettings.h"

#include "Camera/CameraComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void ULBGameUserSettings::ApplyCustomSettings(bool bCheckForCommandLineOverrides, const UObject* WorldContextObject)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
	
	UE_LOG(LogTemp, Warning, TEXT("ULBGameUserSettings::ApplySettings, ScreenBrightnessValue %f, MotionBlurValue %f, bFPSIndicatorEnabled %d"), ScreenBrightnessValue, MotionBlurValue, bFPSIndicatorEnabled)

	if (WorldContextObject)
	{
		if (APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(UGameplayStatics::GetActorOfClass(WorldContextObject, APostProcessVolume::StaticClass())))
		{
			FPostProcessSettings& PostProcessSettings =  PostProcessVolume->Settings;
		
			/* Screen Brightness */
			PostProcessSettings.bOverride_AutoExposureBias = true;
			PostProcessSettings.AutoExposureBias = UKismetMathLibrary::MapRangeClamped(ScreenBrightnessValue, 0.f, 100.f, -1.f, 3.f); 

			/* Motion Blur */
			PostProcessSettings.bOverride_MotionBlurAmount = true;
			PostProcessSettings.MotionBlurAmount = MotionBlurValue;			// 0.0(꺼짐) ~ 1.0(매우강함)
			PostProcessSettings.bOverride_MotionBlurMax = true;
			PostProcessSettings.MotionBlurMax = MotionBlurValue;			// 최대 강도
			PostProcessSettings.bOverride_MotionBlurPerObjectSize = true;
			PostProcessSettings.MotionBlurPerObjectSize = MotionBlurValue;  // 개체 크기에 따른 모션 블러의 변화량
		}
	}

	/* FPS Indicator */
	// TODO : 구현
}

void ULBGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	/* UGameUserSettings */
	// TODO : 초기 게임 설정 초기화 필요
	
	/* ULBGameUserSettings */
	bFPSIndicatorEnabled = false;
	ScreenBrightnessValue = 50.f;
	MotionBlurValue = 0.f;
}
