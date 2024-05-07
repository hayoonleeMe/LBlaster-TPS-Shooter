// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainMenuHUD.h"

#include "MultiplayerSessionsSubsystem.h"
#include "GraphicSettingMenuForMainMenu.h"
#include "MainMenu.h"
#include "SessionListMenu.h"
#include "SettingMenuForMainMenu.h"
#include "StartMenu.h"
#include "Blueprint/UserWidget.h"

void AMainMenuHUD::InitializeMultiplayerSessions(int32 InNumPublicConnections, const FString& InMatchType, const FString& InLobbyPath)
{
	// MultiplayerSessionsSubsystem
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->SessionSetup(InNumPublicConnections, InMatchType, InLobbyPath);
		MultiplayerSessionsSubsystem->LBOnCreateSessionCompleteDelegate.AddUObject(this, &ThisClass::OnCreateSessionComplete);
		MultiplayerSessionsSubsystem->LBOnFindSessionsCompleteDelegate.AddUObject(this, &ThisClass::OnFindSessionsComplete);
		MultiplayerSessionsSubsystem->LBOnJoinSessionCompleteDelegate.AddUObject(this, &ThisClass::OnJoinSessionComplete);
	}
}

void AMainMenuHUD::CreateStartMenu()
{
	if (StartMenuClass && !StartMenu)
	{
		if (IsValidOwnerController())
		{
			StartMenu = CreateWidget<UStartMenu>(OwnerController, StartMenuClass);
		}
	}

	AddNewMenuToStack(StartMenu);
}

void AMainMenuHUD::CreateSettingMenu()
{
	if (SettingMenuClass && !SettingMenu)
	{
		if (IsValidOwnerController())
		{
			SettingMenu = CreateWidget<USettingMenuForMainMenu>(OwnerController, SettingMenuClass);
		}
	}

	AddNewMenuToStack(SettingMenu);
}

void AMainMenuHUD::CreateGraphicSettingMenu()
{
	if (GraphicSettingMenuClass && !GraphicSettingMenu)
	{
		if (IsValidOwnerController())
		{
			GraphicSettingMenu = CreateWidget<UGraphicSettingMenuForMainMenu>(OwnerController, GraphicSettingMenuClass);
		}
	}

	AddNewMenuToStack(GraphicSettingMenu);
}

void AMainMenuHUD::CreateSessionListMenu()
{
	if (SessionListMenuClass && !SessionListMenu)
	{
		if (IsValidOwnerController())
		{
			SessionListMenu = CreateWidget<USessionListMenu>(OwnerController, SessionListMenuClass);
		}
	}

	AddNewMenuToStack(SessionListMenu);
}

void AMainMenuHUD::ReturnMenu()
{
	int32 LastMenuIndex = MenuStack.Num() - 1;
	if (MenuStack.IsValidIndex(LastMenuIndex))
	{
		MenuStack[LastMenuIndex]->MenuTearDown();
		MenuStack.RemoveAt(LastMenuIndex);
		--LastMenuIndex;
	}

	// Main Menu로 돌아감
	if (MenuStack.Num() == 0)
	{
		MainMenu->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		MenuStack[LastMenuIndex]->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainMenuHUD::OnCreateSessionButtonClicked()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession();
	}
}

void AMainMenuHUD::OnFindSessionsButtonClicked()
{
	// Session List Menu 생성
	CreateSessionListMenu();
	
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void AMainMenuHUD::RefreshSessionList()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void AMainMenuHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AddMainMenu();
}

void AMainMenuHUD::OnCreateSessionComplete(bool bWasSuccessful)
{
	if (StartMenu)
	{
		StartMenu->OnCreateSessionComplete(bWasSuccessful);
	}
}

void AMainMenuHUD::OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (SessionListMenu)
	{
		SessionListMenu->InitializeSessionListView(SessionResults, bWasSuccessful);
	}
}

void AMainMenuHUD::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	if (StartMenu)
	{
		StartMenu->OnJoinSessionComplete(Result);
	}
}

void AMainMenuHUD::AddMainMenu()
{
	if (MainMenuClass && !MainMenu)
	{
		if (IsValidOwnerController())
		{
			MainMenu = CreateWidget<UMainMenu>(OwnerController, MainMenuClass);
		}
	}

	if (MainMenu)
	{
		MainMenu->MenuSetup();
	}
}

void AMainMenuHUD::AddNewMenuToStack(UMainMenuUserWidget* InNewMenu)
{
	if (!InNewMenu)
	{
		return;
	}
	
	// Main Menu에서 새로운 메뉴 오픈
	if (MenuStack.Num() == 0)
	{
		MainMenu->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		const int32 LastMenuIndex = MenuStack.Num() - 1;
		if (MenuStack.IsValidIndex(LastMenuIndex))
		{
			MenuStack[LastMenuIndex]->SetVisibility(ESlateVisibility::Hidden);
		}	
	}
	
	MenuStack.Emplace(InNewMenu);
	InNewMenu->MenuSetup();
}

bool AMainMenuHUD::IsValidOwnerController()
{
	if (!OwnerController && GetOwningPlayerController())
	{
		OwnerController = Cast<APlayerController>(GetOwningPlayerController());
	}
	return OwnerController != nullptr;
}
