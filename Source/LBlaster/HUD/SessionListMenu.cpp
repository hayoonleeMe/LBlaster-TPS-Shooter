// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SessionListMenu.h"

#include "MainMenuHUD.h"
#include "OnlineSessionSettings.h"
#include "SessionListRow.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/Overlay.h"

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
	if (AlertReturnButton && !AlertReturnButton->OnClicked.IsBound())
	{
		AlertReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}
}

void USessionListMenu::InitializeSessionListView(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
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

	// 로딩 효과 끄기
	SetLoadingOverlayVisibility(false);

	if (SessionListView && IsValidOwnerController())
	{
		for (const FOnlineSessionSearchResult& SessionResult : SessionResults)
		{
			for (int32 i = 0; i < 20; ++i)	// TODO : Remove Test Code
			{
				if (USessionListRow* SessionListRow = CreateWidget<USessionListRow>(OwnerController, USessionListRow::StaticClass()))
				{
					SessionListRow->SetHostNameString(i % 2 == 0 ? SessionResult.Session.OwningUserName : TEXT("Test Name")); // TODO : Remove Test Code
					FString MatchTypeString;
					SessionResult.Session.SessionSettings.Get(FName(TEXT("MatchType")), MatchTypeString);
					SessionListRow->SetMatchModeString(MatchTypeString);
					SessionListRow->SetNumOfCurrentPlayers(SessionResult.Session.SessionSettings.NumPublicConnections - SessionResult.Session.NumOpenPublicConnections);
					SessionListRow->SetNumOfMaxPlayers(SessionResult.Session.SessionSettings.NumPublicConnections);

					SessionListView->AddItem(SessionListRow);
				}	
			}
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
}

void USessionListMenu::OnReturnButtonClicked()
{
	if (SessionListView)
	{
		SessionListView->ClearListItems();
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
		OwnerHUD->RefreshSessionList();
	}
}
