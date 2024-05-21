// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenu.h"

#include "DropDown.h"
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
	if (GameModeSelector && !GameModeSelector->OnSwitcherActiveIndexChanged.IsBound())
	{
		GameModeSelector->OnSwitcherActiveIndexChanged.BindUObject(this, &ThisClass::OnGameModeSelectorChanged);
	}
	if (MaxPlayerDropDown)
	{
		MaxPlayerDropDown->InitializeOptions(GetMatchModeType());
	}
	if (CreateSessionAlertCreateButton && !CreateSessionAlertCreateButton->OnClicked.IsBound())
	{
		CreateSessionAlertCreateButton->OnClicked.AddDynamic(this, &ThisClass::OnCreateSessionAlertCreateButtonClicked);
	}
	if (CreateSessionAlertCancelButton && !CreateSessionAlertCancelButton->OnClicked.IsBound())
	{
		CreateSessionAlertCancelButton->OnClicked.AddDynamic(this, &ThisClass::OnCreateSessionAlertCancelButtonClicked);
	}

	/* Create Session Fail Alert Overlay */
	if (CreateFailAlertReturnButton && !CreateFailAlertReturnButton->OnClicked.IsBound())
	{
		CreateFailAlertReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnCreateFailAlertReturnButton);
	}
}

bool UStartMenu::IsOverlayOpened()
{
	if (CreateSessionFailAlertOverlay && CreateSessionFailAlertOverlay->IsVisible())
	{
		return true;
	}
	if (CreateSessionAlertOverlay && CreateSessionAlertOverlay->IsVisible())
	{
		return true;
	}
	return false;
}

void UStartMenu::CloseOverlay()
{
	if (CreateSessionFailAlertOverlay && CreateSessionFailAlertOverlay->IsVisible())
	{
		OnCreateFailAlertReturnButton();
	}
	else if (CreateSessionAlertOverlay && CreateSessionAlertOverlay->IsVisible())
	{
		OnCreateSessionAlertCancelButtonClicked();
	}
}

bool UStartMenu::CanReturn()
{
	if (LoadingOverlay && LoadingOverlay->IsVisible())
	{
		return false;
	}
	return true;
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
		if (AMainMenuHUD* MainMenuHUD = Cast<AMainMenuHUD>(OwnerHUD))
		{
			MainMenuHUD->FindSessionsFromMenu();
		}
	}
}

void UStartMenu::OnReturnButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu();
	}		
}

void UStartMenu::OnGameModeSelectorChanged(int32 InActiveIndex)
{
	// MaxPlayerDropDown은 게임 모드에 따라 다른 Option을 표시
	if (MaxPlayerDropDown)
	{
		MaxPlayerDropDown->InitializeOptions(GetMatchModeType());
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

int32 UStartMenu::GetMaxPlayerValue() const
{
	if (MaxPlayerDropDown)
	{
		return MaxPlayerDropDown->GetSelectedValue();
	}
	return 0;
}

void UStartMenu::OnCreateSessionAlertCreateButtonClicked()
{
	SetLoadingOverlayVisibility(true);
	
	const EMatchMode MatchModeType = GetMatchModeType();
	const int32 NumMaxPlayer = GetMaxPlayerValue();
	
	if (IsValidOwnerHUD())
	{
		if (AMainMenuHUD* MainMenuHUD = Cast<AMainMenuHUD>(OwnerHUD))
		{
			MainMenuHUD->CreateSessionFromMenu(MatchModeType, NumMaxPlayer);
		}
	}
}

void UStartMenu::OnCreateSessionAlertCancelButtonClicked()
{
	// Input 위젯 초기화
	if (GameModeSelector)
	{
		GameModeSelector->InitializeOptions();
	}
	if (MaxPlayerDropDown)
	{
		MaxPlayerDropDown->InitializeOptions(GetMatchModeType());
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
