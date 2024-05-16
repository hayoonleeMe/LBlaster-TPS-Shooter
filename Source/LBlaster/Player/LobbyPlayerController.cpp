// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"

#include "LBlaster.h"
#include "HUD/LobbyHUD.h"

void ALobbyPlayerController::ClientSendAddPlayerList_Implementation(ETeam InTeam, const FString& InName)
{
	if (ALobbyHUD* LobbyHUD = GetHUD<ALobbyHUD>())
	{
		LobbyHUD->AddNewPlayerForClient(InTeam, InName);
	}
}

void ALobbyPlayerController::ClientSendTeamChangePlayerList_Implementation(ETeam CurrentTeam, ETeam NewTeam, const FString& InName)
{
	if (ALobbyHUD* LobbyHUD = GetHUD<ALobbyHUD>())
	{
		LobbyHUD->ChangePlayerTeamForClient(CurrentTeam, NewTeam, InName);
	}
}

void ALobbyPlayerController::ServerSendTeamChangePlayerList_Implementation(ETeam CurrentTeam, ETeam NewTeam, ALBlasterPlayerState* InPlayerState)
{
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		if (ALobbyHUD* LobbyHUD = GetWorld()->GetFirstPlayerController()->GetHUD<ALobbyHUD>())
		{
			LobbyHUD->ChangePlayerTeamFromClient(CurrentTeam, NewTeam, InPlayerState);
		}	
	}
}
