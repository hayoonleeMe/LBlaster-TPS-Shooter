// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenu.h"

#include "MainMenu.h"
#include "OptionSelector.h"
#include "SliderWithNumericInput.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "HUD/MainMenuHUD.h"

void UStartMenu::MenuSetup()
{
	Super::MenuSetup();

	/* Widgets */
	if (CreateSessionButton && !CreateSessionButton->OnClicked.IsBound())
	{
		CreateSessionButton->OnClicked.AddDynamic(this, &ThisClass::OnCreateSessionButtonClicked);
	}
	if (FindSessionsButton && !FindSessionsButton->OnClicked.IsBound())
	{
		FindSessionsButton->OnClicked.AddDynamic(this, &ThisClass::OnFindSessionsButtonClicked);
	}
	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}

	/* Create Session Alert Overlay */
	if (MaxPlayerSlider)
	{
		MaxPlayerSlider->InitializeValues(SliderInitialValue, SliderMinValue, SliderMaxValue, SliderStepSize);
	}
	if (AlertCreateSessionButton && !AlertCreateSessionButton->OnClicked.IsBound())
	{
		AlertCreateSessionButton->OnClicked.AddDynamic(this, &ThisClass::OnAlertCreateSessionButtonClicked);
	}
	if (AlertCancelButton && !AlertCancelButton->OnClicked.IsBound())
	{
		AlertCancelButton->OnClicked.AddDynamic(this, &ThisClass::OnAlertCancelButtonClicked);
	}

	/* Create Session Fail Alert Overlay */
	if (CreateFailAlertReturnButton && !CreateFailAlertReturnButton->OnClicked.IsBound())
	{
		CreateFailAlertReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnCreateFailAlertReturnButton);
	}
}

void UStartMenu::OnCreateSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		SetLoadingOverlayVisibility(false);

		if (CreateSessionFailAlertOverlay)
		{
			CreateSessionFailAlertOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UStartMenu::OnCreateSessionButtonClicked()
{
	// Create Session Alert Overlay 표시
	if (CreateSessionAlertOverlay)
	{
		CreateSessionAlertOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void UStartMenu::OnFindSessionsButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->FindSessionsFromMenu();
	}
}

void UStartMenu::OnReturnButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu();
	}		
}

EMatchMode UStartMenu::GetMatchModeType()
{
	if (GameModeSelector)
	{
		const int32 ActiveIndex = GameModeSelector->GetActiveIndex();
		
		// 개인전
		if (ActiveIndex == 0)
		{
			return EMatchMode::FreeForAll;
		}
		// 팀 데스매치
		if (ActiveIndex == 1)
		{
			return EMatchMode::TeamDeathMatch;
		}
	}
	return EMatchMode();
}

int32 UStartMenu::GetMaxPlayerValue()
{
	if (MaxPlayerSlider)
	{
		return FMath::RoundToInt32(MaxPlayerSlider->GetSliderValue());
	}
	return 0;
}

void UStartMenu::OnAlertCreateSessionButtonClicked()
{
	SetLoadingOverlayVisibility(true);
	
	const EMatchMode MatchModeType = GetMatchModeType();
	const int32 NumMaxPlayer = GetMaxPlayerValue();
	
	if (IsValidOwnerHUD())
	{
		OwnerHUD->CreateSessionFromMenu(MatchModeType, NumMaxPlayer);
	}
}

void UStartMenu::OnAlertCancelButtonClicked()
{
	// Input 위젯 초기화
	if (GameModeSelector)
	{
		GameModeSelector->InitializeOptions();
	}
	if (MaxPlayerSlider)
	{
		MaxPlayerSlider->InitializeValues(SliderInitialValue, SliderMinValue, SliderMaxValue, SliderStepSize);
	}

	// Create Session Alert Overlay 숨김
	if (CreateSessionAlertOverlay)
	{
		CreateSessionAlertOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UStartMenu::SetLoadingOverlayVisibility(bool bShow)
{
	if (LoadingOverlay)
	{
		LoadingOverlay->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UStartMenu::OnCreateFailAlertReturnButton()
{
	if (CreateSessionFailAlertOverlay)
	{
		CreateSessionFailAlertOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}
