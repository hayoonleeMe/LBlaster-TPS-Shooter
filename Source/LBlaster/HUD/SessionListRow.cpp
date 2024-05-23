// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SessionListRow.h"

#include "MultiplayerMapPath.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Border.h"
#include "Components/ListViewBase.h"
#include "Components/TextBlock.h"

static FLinearColor NormalColor(0.f, 0.f, 0.f, 0.6f);
static FLinearColor SelectedColor(0.34375f, 0.34375f, 0.328805f, 0.5f);

void USessionListRow::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (USessionListRow* Data = Cast<USessionListRow>(ListItemObject))
	{
		SetSessionResult(Data->SessionResult);
		bClicked = Data->bClicked;

		if (RowBorder)
		{
			RowBorder->SetBrushColor(bClicked ? SelectedColor : NormalColor);
		}
		if (HostNameTextBlock)
		{
			HostNameTextBlock->SetText(FText::FromString(HostNameString));
		}
		if (MatchModeTextBlock)
		{
			if (MatchModeType == EMatchMode::FreeForAll)
			{
				MatchModeTextBlock->SetText(FText::FromString(FString(TEXT("개인전"))));
			}
			else if (MatchModeType == EMatchMode::TeamDeathMatch)
			{
				MatchModeTextBlock->SetText(FText::FromString(FString(TEXT("팀 데스매치"))));
			}
		}
		if (NumPlayerTextBlock)
		{
			NumPlayerTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), NumOfCurrentPlayers, NumOfMaxPlayers)));
		}
	}
}

void USessionListRow::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	bClicked = bIsSelected;
	if (RowBorder)
	{
		RowBorder->SetBrushColor(bIsSelected ? SelectedColor : NormalColor);
	}
}

void USessionListRow::SetSessionResult(const FOnlineSessionSearchResult& InSessionResult)
{
	SessionResult = InSessionResult;
	HostNameString = SessionResult.Session.OwningUserName;
	
	int32 IntMatchModeType;
	SessionResult.Session.SessionSettings.Get(FMultiplayerMapPath::MatchModeKey, IntMatchModeType);
	MatchModeType = static_cast<EMatchMode>(IntMatchModeType);
	
	NumOfCurrentPlayers = SessionResult.Session.SessionSettings.NumPublicConnections - SessionResult.Session.NumOpenPublicConnections;
	NumOfMaxPlayers = SessionResult.Session.SessionSettings.NumPublicConnections;
}
