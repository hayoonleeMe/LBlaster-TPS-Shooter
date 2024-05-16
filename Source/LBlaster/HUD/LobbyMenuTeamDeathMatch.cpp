// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LobbyMenuTeamDeathMatch.h"

#include "LobbyHUD.h"
#include "PlayerListRow.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "LBTypes/Team.h"
#include "Player/LBlasterPlayerState.h"
#include "Player/LobbyPlayerController.h"

void ULobbyMenuTeamDeathMatch::MenuSetup()
{
	Super::MenuSetup();

	if (RedTeamButton && !RedTeamButton->OnClicked.IsBound())
	{
		RedTeamButton->OnClicked.AddDynamic(this, &ThisClass::OnRedTeamButtonClicked);
	}
	if (BlueTeamButton && !BlueTeamButton->OnClicked.IsBound())
	{
		BlueTeamButton->OnClicked.AddDynamic(this, &ThisClass::OnBlueTeamButtonClicked);
	}
}

void ULobbyMenuTeamDeathMatch::AddNewPlayer(ALBlasterPlayerState* InPlayerState)
{
	if (RedTeamBox && BlueTeamBox && InPlayerState)
	{
		const int32 NumRedTeam = RedTeamBox->GetSlots().Num();
		const int32 NumBlueTeam = BlueTeamBox->GetSlots().Num();

		if (NumRedTeam <= NumBlueTeam)
		{
			AddRedTeam(InPlayerState, true);
		}
		else
		{
			AddBlueTeam(InPlayerState, true);
		}
		
		AddNumCurrentPlayersText(1);
	}
}

void ULobbyMenuTeamDeathMatch::AddNewPlayerForClient(ETeam InTeam, const FString& InName)
{
	if (InTeam == ETeam::ET_RedTeam)
	{
		AddRedTeamForClient(InName);
	}
	else if (InTeam == ETeam::ET_BlueTeam)
	{
		AddBlueTeamForClient(InName);
	}

	AddNumCurrentPlayersText(1);
}

void ULobbyMenuTeamDeathMatch::AddRedTeam(ALBlasterPlayerState* InPlayerState, bool bDoBroadcast)
{
	if (RedTeamBox && IsValidOwnerController() && PlayerListRowClass && InPlayerState && IsValidOwnerPlayerState())
	{
		if (UPlayerListRow* PlayerListRow = CreateWidget<UPlayerListRow>(OwnerController, PlayerListRowClass))
		{
			InPlayerState->SetTeam(ETeam::ET_RedTeam);
			PlayerListRow->SetNameText(InPlayerState->GetPlayerName());
			// 로컬 플레이어 이름 강조 표시
			if (OwnerPlayerState->GetPlayerName() == InPlayerState->GetPlayerName())
			{
				PlayerListRow->HighlightNameText();
			}
			RedTeamBox->AddChildToVerticalBox(PlayerListRow);
			if (bDoBroadcast)
			{
				BroadcastAddPlayerList(ETeam::ET_RedTeam, InPlayerState->GetPlayerName());
			}
		}
	}
}

void ULobbyMenuTeamDeathMatch::AddRedTeamForClient(const FString& InName)
{
	if (RedTeamBox && IsValidOwnerController() && PlayerListRowClass && IsValidOwnerPlayerState())
    {
    	if (UPlayerListRow* PlayerListRow = CreateWidget<UPlayerListRow>(OwnerController, PlayerListRowClass))
    	{
    		PlayerListRow->SetNameText(InName);
    		// 로컬 플레이어 이름 강조 표시
    		if (OwnerPlayerState->GetPlayerName() == InName)
    		{
    			PlayerListRow->HighlightNameText();
    		}
    		RedTeamBox->AddChildToVerticalBox(PlayerListRow);
    	}
    }
}

void ULobbyMenuTeamDeathMatch::AddBlueTeam(ALBlasterPlayerState* InPlayerState, bool bDoBroadcast)
{
	if (BlueTeamBox && IsValidOwnerController() && PlayerListRowClass && InPlayerState && IsValidOwnerPlayerState())
	{
		if (UPlayerListRow* PlayerListRow = CreateWidget<UPlayerListRow>(OwnerController, PlayerListRowClass))
		{
			InPlayerState->SetTeam(ETeam::ET_BlueTeam);
			PlayerListRow->SetNameText(InPlayerState->GetPlayerName());
			// 로컬 플레이어 이름 강조 표시
			if (OwnerPlayerState->GetPlayerName() == InPlayerState->GetPlayerName())
			{
				PlayerListRow->HighlightNameText();
			}
			BlueTeamBox->AddChildToVerticalBox(PlayerListRow);
			if (bDoBroadcast)
			{
				BroadcastAddPlayerList(ETeam::ET_BlueTeam, InPlayerState->GetPlayerName());
			}
		}
	}
}

void ULobbyMenuTeamDeathMatch::AddBlueTeamForClient(const FString& InName)
{
	if (BlueTeamBox && IsValidOwnerController() && PlayerListRowClass && IsValidOwnerPlayerState())
	{
		if (UPlayerListRow* PlayerListRow = CreateWidget<UPlayerListRow>(OwnerController, PlayerListRowClass))
		{
			PlayerListRow->SetNameText(InName);
			// 로컬 플레이어 이름 강조 표시
			if (OwnerPlayerState->GetPlayerName() == InName)
			{
				PlayerListRow->HighlightNameText();
			}
			BlueTeamBox->AddChildToVerticalBox(PlayerListRow);
		}
	}
}

void ULobbyMenuTeamDeathMatch::ChangeTeam(ETeam CurrentTeam, ETeam NewTeam, ALBlasterPlayerState* InPlayerState)
{
	// 기존 PlayerListRow 제거
	if (CurrentTeam == ETeam::ET_RedTeam)
	{
		RemovePlayerFromRedTeam(InPlayerState->GetPlayerName(), false);
	}
	else if (CurrentTeam == ETeam::ET_BlueTeam)
	{
		RemovePlayerFromBlueTeam(InPlayerState->GetPlayerName(), false);
	}

	// 새로운 팀에 추가
	if (IsValidOwnerPlayerState())
	{
		if (NewTeam == ETeam::ET_RedTeam)
		{
			AddRedTeam(InPlayerState, false);
		}
		else if (NewTeam == ETeam::ET_BlueTeam)
		{
			AddBlueTeam(InPlayerState, false);
		}	
	}

	BroadcastTeamChangePlayerList(CurrentTeam, NewTeam, InPlayerState->GetPlayerName());
}

void ULobbyMenuTeamDeathMatch::ChangeTeamForClient(ETeam CurrentTeam, ETeam NewTeam, const FString& InName)
{
	// 기존 PlayerListRow 제거
	if (CurrentTeam == ETeam::ET_RedTeam)
	{
		RemovePlayerFromRedTeam(InName, false);
	}
	else if (CurrentTeam == ETeam::ET_BlueTeam)
	{
		RemovePlayerFromBlueTeam(InName, false);
	}

	// 새로운 팀에 추가
	if (IsValidOwnerPlayerState())
	{
		if (NewTeam == ETeam::ET_RedTeam)
		{
			AddRedTeamForClient(InName);
		}
		else if (NewTeam == ETeam::ET_BlueTeam)
		{
			AddBlueTeamForClient(InName);
		}	
	}
}

void ULobbyMenuTeamDeathMatch::RemovePlayerFromRedTeam(const FString& InName, bool bDoBroadcast)
{
	if (RedTeamBox)
	{
		for (int32 Index = RedTeamBox->GetSlots().Num() - 1; Index >= 0; --Index)
		{
			if (UPlayerListRow* Row = Cast<UPlayerListRow>(RedTeamBox->GetSlots()[Index]->Content))
			{
				// 해당 Row 삭제
				if (InName == Row->GetPlayerName())
				{
					RedTeamBox->RemoveChildAt(Index);
					if (bDoBroadcast)
					{
						BroadcastRemovePlayerList(ETeam::ET_RedTeam, InName);
					}
					break;
				}
			}
		}
	}
	
}

void ULobbyMenuTeamDeathMatch::RemovePlayerFromBlueTeam(const FString& InName, bool bDoBroadcast)
{
	if (BlueTeamBox)
	{
		for (int32 Index = BlueTeamBox->GetSlots().Num() - 1; Index >= 0; --Index)
		{
			if (UPlayerListRow* Row = Cast<UPlayerListRow>(BlueTeamBox->GetSlots()[Index]->Content))
			{
				// 해당 Row 삭제
				if (InName == Row->GetPlayerName())
				{
					BlueTeamBox->RemoveChildAt(Index);
					if (bDoBroadcast)
					{
						BroadcastRemovePlayerList(ETeam::ET_BlueTeam, InName);
					}
					break;
				}
			}
		}
	}
}

void ULobbyMenuTeamDeathMatch::RemoveLogoutPlayer(ETeam InTeam, const FString& InName, bool bHasAuthority)
{
	if (InTeam == ETeam::ET_RedTeam)
	{
		RemovePlayerFromRedTeam(InName, bHasAuthority);
	}
	else if (InTeam == ETeam::ET_BlueTeam)
	{
		RemovePlayerFromBlueTeam(InName, bHasAuthority);
	}
	AddNumCurrentPlayersText(-1);
}

bool ULobbyMenuTeamDeathMatch::IsValidOwnerPlayerState()
{
	if (!OwnerPlayerState && IsValidOwnerController())
	{
		OwnerPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>();
	}
	return OwnerPlayerState != nullptr;
}

void ULobbyMenuTeamDeathMatch::OnRedTeamButtonClicked()
{
	if (IsValidOwnerPlayerState())
	{
		if (OwnerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			return;
		}
		
		if (OwnerPlayerState->HasAuthority())
		{
			ChangeTeam(OwnerPlayerState->GetTeam(), ETeam::ET_RedTeam, OwnerPlayerState);	
		}
		else
		{
			if (IsValidOwnerController())
			{
				if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(OwnerController))
				{
					LobbyPlayerController->ServerSendTeamChangePlayerList(OwnerPlayerState->GetTeam(), ETeam::ET_RedTeam, OwnerPlayerState);
				}
			}			
		}
	}
}

void ULobbyMenuTeamDeathMatch::OnBlueTeamButtonClicked()
{
	if (IsValidOwnerPlayerState())
	{
		if (OwnerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			return;
		}
		
		if (OwnerPlayerState->HasAuthority())
		{
			ChangeTeam(OwnerPlayerState->GetTeam(), ETeam::ET_BlueTeam, OwnerPlayerState);
		}
		else
		{
			if (IsValidOwnerController())
			{
				if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(OwnerController))
				{
					LobbyPlayerController->ServerSendTeamChangePlayerList(OwnerPlayerState->GetTeam(), ETeam::ET_BlueTeam, OwnerPlayerState);
				}
			}	
		}
	}
}

void ULobbyMenuTeamDeathMatch::BroadcastAddPlayerList(ETeam InTeam, const FString& InName)
{
	// 서버의 PlayerList 추가를 연결된 모든 클라에 브로드캐스팅해 UI 동기화
	if (IsValidOwnerController() && OwnerController->HasAuthority() && IsValidOwnerHUD())
	{
		if (ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(OwnerHUD))
		{
			LobbyHUD->BroadcastAddPlayerList(InTeam, InName);
		}
	}
}

void ULobbyMenuTeamDeathMatch::BroadcastTeamChangePlayerList(ETeam CurrentTeam, ETeam NewTeam, const FString& InName)
{
	// 서버의 PlayerList 팀변경을 연결된 모든 클라에 브로드캐스팅해 UI 동기화
	if (IsValidOwnerController() && OwnerController->HasAuthority() && IsValidOwnerHUD())
	{
		if (ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(OwnerHUD))
		{
			LobbyHUD->BroadcastTeamChangePlayerList(CurrentTeam, NewTeam, InName);
		}
	}
}

void ULobbyMenuTeamDeathMatch::BroadcastRemovePlayerList(ETeam InTeam, const FString& InName)
{
	if (IsValidOwnerController() && OwnerController->HasAuthority() && IsValidOwnerHUD())
	{
		if (ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(OwnerHUD))
		{
			LobbyHUD->BroadcastRemovePlayerList(InTeam, InName);
		}
	}
}
