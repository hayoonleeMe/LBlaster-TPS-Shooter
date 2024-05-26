// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUserSettings/LBGameUserSettings.h"

#include "Camera/CameraComponent.h"
#include "Character/LBlasterCharacter.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LBlasterPlayerController.h"

void ULBGameUserSettings::SetFPSIndicatorEnabled(bool bEnable)
{
	if (bFPSIndicatorEnabled != bEnable)
	{
		bFPSIndicatorEnabled = bEnable;
		OnFPSIndicatorEnabledChanged.Broadcast(bFPSIndicatorEnabled);
	}
}

void ULBGameUserSettings::ApplyCustomSettings(bool bCheckForCommandLineOverrides, const UObject* WorldContextObject)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
	
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

		/* Mouse Sensitivity */
		if (WorldContextObject->GetWorld())
		{
			if (ALBlasterPlayerController* FirstPlayerController = Cast<ALBlasterPlayerController>(WorldContextObject->GetWorld()->GetFirstPlayerController()))
			{
				if (ALBlasterCharacter* LBCharacter = Cast<ALBlasterCharacter>(FirstPlayerController->GetCharacter()))
				{
					const float ClampedXAxisMouseSensitivity = FMath::GetMappedRangeValueClamped(MouseSensitivityInputRange, MouseSensitivityOutputRange, XAxisMouseSensitivity);
					const float ClampedYAxisMouseSensitivity = FMath::GetMappedRangeValueClamped(MouseSensitivityInputRange, MouseSensitivityOutputRange, YAxisMouseSensitivity);
					LBCharacter->SetXAxisSensitivityFromUserSettings(ClampedXAxisMouseSensitivity);
					LBCharacter->SetYAxisSensitivityFromUserSettings(ClampedYAxisMouseSensitivity);
				}
			}
		}
	}
}

void ULBGameUserSettings::SetGraphicOptionByAutoDetect(bool bFirstExecute)
{
	RunHardwareBenchmark();
	
	// Auto-Detect를 해도 Resolution Scale은 항상 100으로 고정
	SetResolutionScaleValueEx(100.f);

	// 첫 실행일 때 기본 설정
	if (bFirstExecute)
	{
		SetScreenResolution(FIntPoint{ 1920, 1080 });
		SetFullscreenMode(EWindowMode::WindowedFullscreen);
		SetXAxisMouseSensitivity(50.f);
		SetYAxisMouseSensitivity(50.f);
		SetFrameRateLimit(120.f);
		bFPSIndicatorEnabled = true;
		ApplySettings(false);
	}
	
	ApplyHardwareBenchmarkResults();
	
	// 하드웨어에 맞춰 자동으로 설정된 설정에 따라 커스텀 프리셋 값 저장
	const int32 AAQuality = GetAntiAliasingQuality();
	if (AAQuality == GetViewDistanceQuality() && AAQuality == GetShadowQuality() && AAQuality == GetGlobalIlluminationQuality() && AAQuality == GetReflectionQuality() && AAQuality == GetPostProcessingQuality() && AAQuality == GetTextureQuality() && AAQuality == GetVisualEffectQuality() && AAQuality == GetFoliageQuality() && AAQuality == GetShadingQuality())
	{
		GraphicPresetValue = AAQuality;
	}
	else
	{
		// 커스텀
		GraphicPresetValue = 5;
	}

	// 한번 더 저장.
	SaveSettings();
}
