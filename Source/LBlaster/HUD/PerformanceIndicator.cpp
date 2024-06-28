// Fill out your copyright notice in the Description page of Project Settings.


#include "PerformanceIndicator.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "GameUserSettings/LBGameUserSettings.h"

void UPerformanceIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	/* Performance Indicator */
	if (ULBGameUserSettings* GameUserSettings = Cast<ULBGameUserSettings>(GEngine->GetGameUserSettings()))
	{
		GameUserSettings->OnPerformanceIndicatorEnabledChanged.AddUObject(this, &ThisClass::OnPerformanceIndicatorEnabledChanged);

		OnPerformanceIndicatorEnabledChanged(GameUserSettings->IsEnabledPerformanceIndicator());
	}
}

void UPerformanceIndicator::OnPerformanceIndicatorEnabledChanged(bool bPerformanceIndicatorEnabled)
{
	// Performance Indicator Text Block 업데이트
	if (FPSIndicatorText && PingIndicatorText && GetWorld())
	{
		if (bPerformanceIndicatorEnabled)
		{
			SetVisibility(ESlateVisibility::Visible);
			GetWorld()->GetTimerManager().SetTimer(PerformanceIndicatorTimer, this, &ThisClass::UpdatePerformanceIndicator, PerformanceIndicatorTimerRate, true);			
		}
		else
		{
			SetVisibility(ESlateVisibility::Collapsed);
			GetWorld()->GetTimerManager().ClearTimer(PerformanceIndicatorTimer);
		}
	}
}

void UPerformanceIndicator::UpdatePerformanceIndicator()
{
	// FPS Indicator
	if (FPSIndicatorText && FPSIndicatorText->IsVisible() && GetWorld())
	{
		const int32 FPSRate = FMath::RoundToInt32(1.f / GetWorld()->GetDeltaSeconds());

		if (CurrentFPSRate != FPSRate)
		{
			CurrentFPSRate = FPSRate;
			FPSIndicatorText->SetText(FText::AsNumber(CurrentFPSRate));
		}
	}

	// Ping Indicator
	if (PingIndicatorText && PingIndicatorText->IsVisible() && GetWorld() && IsValidOwnerController())
	{
		if (APlayerState* PlayerState = OwnerController->GetPlayerState<APlayerState>())
		{
			const int32 Ping = FMath::RoundToInt32(PlayerState->GetPingInMilliseconds());

			if (CurrentPing != Ping)
			{
				CurrentPing = Ping;
				PingIndicatorText->SetText(FText::AsNumber(CurrentPing));
			}
		}
	}
}
