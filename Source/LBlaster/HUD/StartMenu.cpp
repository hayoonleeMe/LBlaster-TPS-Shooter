// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenu.h"

#include "MainMenu.h"
#include "Components/Button.h"
#include "HUD/MainMenuHUD.h"

void UStartMenu::MenuSetup()
{
	Super::MenuSetup();

	/* Widgets */
	if (HostButton && !HostButton->OnClicked.IsBound())
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::OnHostButtonClicked);
	}
	if (JoinButton && !JoinButton->OnClicked.IsBound())
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
	}
	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}
}

void UStartMenu::OnCreateSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		if (HostButton)
		{
			HostButton->SetIsEnabled(true);
		}
	}
}

void UStartMenu::OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!bWasSuccessful || SessionResults.Num() <= 0)
	{
		if (JoinButton)
		{
			JoinButton->SetIsEnabled(true);
		}
	}
}

void UStartMenu::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
    {
	    if (JoinButton)
	    {
		    JoinButton->SetIsEnabled(true);
	    }	
    }
}

void UStartMenu::OnHostButtonClicked()
{
	if (HostButton)
	{
		HostButton->SetIsEnabled(false);
	}

	if (IsValidOwnerHUD())
	{
		OwnerHUD->OnHostButtonClicked();
	}
}

void UStartMenu::OnJoinButtonClicked()
{
	if (JoinButton)
	{
		JoinButton->SetIsEnabled(false);
	}
	
	if (IsValidOwnerHUD())
	{
		OwnerHUD->OnJoinButtonClicked();
	}
}

void UStartMenu::OnReturnButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu();
	}		
}
