// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SessionListMenu.h"

#include "MainMenuHUD.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "SessionListRow.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/Overlay.h"
#include "GameInstance/LBGameInstance.h"
#include "Player/LBlasterPlayerState.h"

void USessionListMenu::MenuSetup()
{
	Super::MenuSetup();

	// 로딩 효과 표시
	SetLoadingOverlayVisibility(true);
	
	if (JoinButton)
	{
		JoinButton->SetIsEnabled(false);
		
		if (!JoinButton->OnClicked.IsBound())
		{
			JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
		}
	}
	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}
	if (RefreshButton && !RefreshButton->OnClicked.IsBound())
	{
		RefreshButton->OnClicked.AddDynamic(this, &ThisClass::OnRefreshButtonClicked);
	}
	if (FindFailAlertReturnButton && !FindFailAlertReturnButton->OnClicked.IsBound())
	{
		FindFailAlertReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}
	if (JoinFailAlertReturnButton && !JoinFailAlertReturnButton->OnClicked.IsBound())
	{
		JoinFailAlertReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}
}

bool USessionListMenu::IsOverlayOpened()
{
	if (JoinSessionFailAlertOverlay && JoinSessionFailAlertOverlay->IsVisible())
	{
		return true;
	}
	if (FindSessionsFailAlertOverlay && FindSessionsFailAlertOverlay->IsVisible())
	{
		return true;
	}
	return false;
}

void USessionListMenu::CloseOverlay()
{
	if ((JoinSessionFailAlertOverlay && JoinSessionFailAlertOverlay->IsVisible()) || (FindSessionsFailAlertOverlay && FindSessionsFailAlertOverlay->IsVisible()))
	{
		OnReturnButtonClicked();
	}
}

bool USessionListMenu::CanReturn()
{
	if (LoadingOverlay && LoadingOverlay->IsVisible())
	{
		return false;
	}
	return true;
}

void USessionListMenu::InitializeSessionListView(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	// 로딩 효과 끄기
	SetLoadingOverlayVisibility(false);
	
	// Find Session 실패
	if (!bWasSuccessful)
	{
		// Fail Alert 표시
		if (FindSessionsFailAlertOverlay)
		{
			FindSessionsFailAlertOverlay->SetVisibility(ESlateVisibility::Visible);
		}
		return;
	}

	if (SessionListView && IsValidOwnerController())
	{
		for (const FOnlineSessionSearchResult& SessionResult : SessionResults)
		{
			// 이 게임에서 사용하는 유효한 키를 가지는 세션만 띄움
			int32 Value;
			if (!SessionResult.Session.SessionSettings.Get(UMultiplayerSessionsSubsystem::MatchModeKey, Value))
			{
				continue;
			}
			
			if (USessionListRow* SessionListRow = CreateWidget<USessionListRow>(OwnerController, USessionListRow::StaticClass()))
			{
				SessionListRow->SetSessionResult(SessionResult);
				SessionListView->AddItem(SessionListRow);
			}	
		}
	}
}

void USessionListMenu::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	// Join Session 실패
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		SetLoadingOverlayVisibility(false);
		
		// Fail Alert 표시
		if (JoinSessionFailAlertOverlay)
		{
			JoinSessionFailAlertOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void USessionListMenu::SetLoadingOverlayVisibility(bool bShow)
{
	if (LoadingOverlay)
	{
		LoadingOverlay->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void USessionListMenu::OnListViewItemSelectionChanged(bool bIsSelected)
{
	// Selection Mode가 Single Toggle이므로 bIsSelected 만으로 Item이 선택됐는지 유무를 알 수 있다.
	if (JoinButton)
	{
		JoinButton->SetIsEnabled(bIsSelected);	
	}
}

void USessionListMenu::OnJoinButtonClicked()
{
	// 로딩 효과 표시
	SetLoadingOverlayVisibility(true);

	// TODO : 로비가 구현되면 로비의 값을 사용해야함
	
	if (SessionListView)
	{
		if (USessionListRow* SelectedRow = SessionListView->GetSelectedItem<USessionListRow>())
		{
			if (IsValidOwnerHUD())
			{
				if (AMainMenuHUD* MainMenuHUD = Cast<AMainMenuHUD>(OwnerHUD))
				{
					MainMenuHUD->JoinSessionFromMenu(SelectedRow->GetSessionResult());
				}
			}
		}
	}
}

void USessionListMenu::OnReturnButtonClicked()
{
	if (SessionListView)
	{
		SessionListView->ClearListItems();
	}
	if (FindSessionsFailAlertOverlay)
	{
		FindSessionsFailAlertOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (JoinSessionFailAlertOverlay)
	{
		JoinSessionFailAlertOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu();
	}
}

void USessionListMenu::OnRefreshButtonClicked()
{
	// 로딩 효과 표시
	SetLoadingOverlayVisibility(true);
	
	if (SessionListView)
	{
		SessionListView->ClearListItems();
	}
	if (IsValidOwnerHUD())
	{
		if (AMainMenuHUD* MainMenuHUD = Cast<AMainMenuHUD>(OwnerHUD))
		{
			MainMenuHUD->RefreshSessionList();
		}

	}
}
