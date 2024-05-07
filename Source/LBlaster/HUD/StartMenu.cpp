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
}

void UStartMenu::OnCreateSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		if (CreateSessionButton)
		{
			CreateSessionButton->SetIsEnabled(true);
		}
	}
}

void UStartMenu::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
    {
	    if (FindSessionsButton)
	    {
		    FindSessionsButton->SetIsEnabled(true);
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
		OwnerHUD->OnFindSessionsButtonClicked();
	}
}

void UStartMenu::OnReturnButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu();
	}		
}

void UStartMenu::OnAlertCreateSessionButtonClicked()
{
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
