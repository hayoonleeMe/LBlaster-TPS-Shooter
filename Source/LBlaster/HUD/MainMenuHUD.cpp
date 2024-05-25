// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainMenuHUD.h"

#include "GraphicSettingMenu.h"
#include "MultiplayerSessionsSubsystem.h"
#include "MainMenu.h"
#include "SessionListMenu.h"
#include "SettingMenu.h"
#include "StartMenu.h"
#include "Blueprint/UserWidget.h"

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
			SettingMenu = CreateWidget<USettingMenu>(OwnerController, SettingMenuClass);
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
			GraphicSettingMenu = CreateWidget<UGraphicSettingMenu>(OwnerController, GraphicSettingMenuClass);
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

void AMainMenuHUD::ReturnMenu(bool bForceReturn)
{
	int32 LastMenuIndex = MenuStack.Num() - 1;
	if (MenuStack.IsValidIndex(LastMenuIndex) && MenuStack[LastMenuIndex])
	{
		// 현재 메뉴에서 Return할 수 없는 작업을 수행 중이면 Early Return
		if (!MenuStack[LastMenuIndex]->CanReturn())
		{
			return;
		}
		
		// 이전 메뉴로 이동
		if (!MenuStack[LastMenuIndex]->IsOverlayOpened() || bForceReturn)
		{
			MenuStack[LastMenuIndex]->MenuTearDown();
			MenuStack.RemoveAt(LastMenuIndex);
			--LastMenuIndex;	
		}
		// 열린 Overlay 닫음
		else
		{
			MenuStack[LastMenuIndex]->CloseOverlay();
		}
	}

	// Main Menu로 돌아감
	if (MenuStack.Num() == 0)
	{
		MainMenu->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		if (MenuStack[LastMenuIndex])
		{
			MenuStack[LastMenuIndex]->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AMainMenuHUD::AddNewMenuToStack(ULBlasterUserWidget* InNewMenu)
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
		if (MenuStack.IsValidIndex(LastMenuIndex) && MenuStack[LastMenuIndex])
		{
			MenuStack[LastMenuIndex]->SetVisibility(ESlateVisibility::Hidden);
		}	
	}
	
	MenuStack.Emplace(InNewMenu);
	InNewMenu->MenuSetup();
}

void AMainMenuHUD::CreateSessionFromMenu(EMatchMode InMatchModeType, int32 NumMaxPlayer, int32 InGoalKillScore)
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(InMatchModeType, NumMaxPlayer, InGoalKillScore);
	}
}

void AMainMenuHUD::FindSessionsFromMenu()
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

void AMainMenuHUD::JoinSessionFromMenu(const FOnlineSessionSearchResult& SessionResult)
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->JoinSession(SessionResult);
	}
}

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	/* MultiplayerSessionsSubsystem */
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->LBOnCreateSessionCompleteDelegate.AddUObject(this, &ThisClass::OnCreateSessionComplete);
		MultiplayerSessionsSubsystem->LBOnFindSessionsCompleteDelegate.AddUObject(this, &ThisClass::OnFindSessionsComplete);
		MultiplayerSessionsSubsystem->LBOnJoinSessionCompleteDelegate.AddUObject(this, &ThisClass::OnJoinSessionComplete);
	}

	/* Main Menu */
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
	if (SessionListMenu)
	{
		SessionListMenu->OnJoinSessionComplete(Result);
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

bool AMainMenuHUD::IsValidOwnerController()
{
	if (!OwnerController && GetOwningPlayerController())
	{
		OwnerController = Cast<APlayerController>(GetOwningPlayerController());
	}
	return OwnerController != nullptr;
}
