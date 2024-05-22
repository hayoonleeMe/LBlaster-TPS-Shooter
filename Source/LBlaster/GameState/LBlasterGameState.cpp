// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterGameState.h"

#include "HUD/LBlasterHUD.h"
#include "Net/UnrealNetwork.h"

void ALBlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALBlasterGameState, RedTeam);
	DOREPLIFETIME(ALBlasterGameState, RedTeamScore);
	DOREPLIFETIME(ALBlasterGameState, BlueTeam);
	DOREPLIFETIME(ALBlasterGameState, BlueTeamScore);
}

void ALBlasterGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateScoreboard(true);
			}
		}
	}
}

void ALBlasterGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	if (GetWorld())
    {
    	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    	{
    		if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
    		{
    			HUD->UpdateScoreboard(true);
    		}
    	}
	}	
}

void ALBlasterGameState::OnRep_RedTeam()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateScoreboard(true);
			}
		}
	}
}

void ALBlasterGameState::OnRep_BlueTeam()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateScoreboard(true);
			}
		}
	}
}

void ALBlasterGameState::OnRep_RedTeamScore()
{
}

void ALBlasterGameState::OnRep_BlueTeamScore()
{
}
