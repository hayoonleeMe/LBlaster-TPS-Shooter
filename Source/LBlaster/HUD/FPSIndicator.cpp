// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/FPSIndicator.h"

#include "Components/TextBlock.h"
#include "GameUserSettings/LBGameUserSettings.h"

void UFPSIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	/* FPS Indicator */
	if (ULBGameUserSettings* GameUserSettings = Cast<ULBGameUserSettings>(GEngine->GetGameUserSettings()))
	{
		GameUserSettings->OnFPSIndicatorEnabledChanged.AddUObject(this, &ThisClass::OnFPSIndicatorEnabledChanged);

		OnFPSIndicatorEnabledChanged(GameUserSettings->IsEnabledFPSIndicator());
	}
}

void UFPSIndicator::OnFPSIndicatorEnabledChanged(bool bFPSIndicatorEnabled)
{
	// FPS Indicator Text Block 업데이트
	if (FPSIndicatorText && GetWorld())
	{
		if (bFPSIndicatorEnabled)
		{
			FPSIndicatorText->SetVisibility(ESlateVisibility::Visible);
			GetWorld()->GetTimerManager().SetTimer(FPSIndicatorTimer, this, &ThisClass::UpdateFPSIndicator, FPSIndicatorTimerRate, true);			
		}
		else
		{
			FPSIndicatorText->SetVisibility(ESlateVisibility::Collapsed);
			GetWorld()->GetTimerManager().ClearTimer(FPSIndicatorTimer);
		}
	}
}

void UFPSIndicator::UpdateFPSIndicator()
{
	if (FPSIndicatorText && FPSIndicatorText->IsVisible() && GetWorld())
	{
		const int32 FPSRate = FMath::RoundToInt32(1.f / GetWorld()->GetDeltaSeconds());

		if (CurrentFPSRate != FPSRate)
		{
			CurrentFPSRate = FPSRate;
			FPSIndicatorText->SetText(FText::AsNumber(CurrentFPSRate));
		}
	}
}
