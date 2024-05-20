// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LobbyMenuFreeForAll.h"

#include "LobbyHUD.h"
#include "PlayerListRow.h"
#include "Components/VerticalBox.h"
#include "Player/LBlasterPlayerState.h"

void ULobbyMenuFreeForAll::AddNewPlayer(const FString& InName)
{
	if (LeftBox && RightBox)
	{
		const int32 NumLeftList = LeftBox->GetSlots().Num();
		const int32 NumRightList = RightBox->GetSlots().Num();

		if (NumLeftList <= NumRightList)
		{
			AddLeftList(InName, true);
		}
		else
		{
			AddRightList(InName, true);
		}
		AddNumCurrentPlayersText(1);
	}
}

void ULobbyMenuFreeForAll::AddNewPlayerForClient(const FString& InName)
{
	if (LeftBox && RightBox)
	{
		const int32 NumLeftList = LeftBox->GetSlots().Num();
		const int32 NumRightList = RightBox->GetSlots().Num();

		if (NumLeftList <= NumRightList)
		{
			AddLeftList(InName, false);
		}
		else
		{
			AddRightList(InName, false);
		}
		AddNumCurrentPlayersText(1);
	}
}

void ULobbyMenuFreeForAll::AddLeftList(const FString& InName, bool bDoBroadcast)
{
	if (LeftBox && IsValidOwnerController() && PlayerListRowClass && IsValidOwnerPlayerState())
	{
		if (UPlayerListRow* PlayerListRow = CreateWidget<UPlayerListRow>(OwnerController, PlayerListRowClass))
		{
			PlayerListRow->SetNameText(InName);
			// 로컬 플레이어 이름 강조 표시
			if (OwnerPlayerState->GetPlayerName() == InName)
			{
				PlayerListRow->HighlightNameText();
			}
			LeftBox->AddChildToVerticalBox(PlayerListRow);
			if (bDoBroadcast)
			{
				BroadcastAddPlayerList(InName);
			}
		}
	}
}

void ULobbyMenuFreeForAll::AddRightList(const FString& InName, bool bDoBroadcast)
{
	if (RightBox && IsValidOwnerController() && PlayerListRowClass && IsValidOwnerPlayerState())
	{
		if (UPlayerListRow* PlayerListRow = CreateWidget<UPlayerListRow>(OwnerController, PlayerListRowClass))
		{
			PlayerListRow->SetNameText(InName);
			// 로컬 플레이어 이름 강조 표시
			if (OwnerPlayerState->GetPlayerName() == InName)
			{
				PlayerListRow->HighlightNameText();
			}
			RightBox->AddChildToVerticalBox(PlayerListRow);
			if (bDoBroadcast)
			{
				BroadcastAddPlayerList(InName);
			}
		}
	}
}

void ULobbyMenuFreeForAll::RemoveLogoutPlayer(const FString& InName, bool bHasAuthority)
{
	if (LeftBox)
	{
		for (int32 Index = LeftBox->GetSlots().Num() - 1; Index >= 0; --Index)
		{
			if (UPlayerListRow* Row = Cast<UPlayerListRow>(LeftBox->GetSlots()[Index]->Content))
			{
				// 해당 Row 삭제
				if (InName == Row->GetPlayerName())
				{
					LeftBox->RemoveChildAt(Index);
					if (bHasAuthority)
					{
						BroadcastRemovePlayerList(InName);
					}
					break;
				}
			}
		}
	}
	if (RightBox)
	{
		for (int32 Index = RightBox->GetSlots().Num() - 1; Index >= 0; --Index)
		{
			if (UPlayerListRow* Row = Cast<UPlayerListRow>(RightBox->GetSlots()[Index]->Content))
			{
				// 해당 Row 삭제
				if (InName == Row->GetPlayerName())
				{
					RightBox->RemoveChildAt(Index);
					if (bHasAuthority)
					{
						BroadcastRemovePlayerList(InName);
					}
					break;
				}
			}
		}
	}
	AddNumCurrentPlayersText(-1);
}

void ULobbyMenuFreeForAll::BroadcastAddPlayerList(const FString& InName)
{
	// 서버의 PlayerList 추가를 연결된 모든 클라에 브로드캐스팅해 UI 동기화
	if (IsValidOwnerController() && OwnerController->HasAuthority() && IsValidOwnerHUD())
	{
		if (ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(OwnerHUD))
		{
			LobbyHUD->BroadcastAddPlayerList(InName);
		}
	}
}

void ULobbyMenuFreeForAll::BroadcastRemovePlayerList(const FString& InName)
{
	if (IsValidOwnerController() && OwnerController->HasAuthority() && IsValidOwnerHUD())
	{
		if (ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(OwnerHUD))
		{
			LobbyHUD->BroadcastRemovePlayerList(InName);
		}
	}
}
