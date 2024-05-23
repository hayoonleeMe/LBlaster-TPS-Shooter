// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamDeathMatchGameState.h"

#include "HUD/LBlasterHUD.h"
#include "Net/UnrealNetwork.h"

void ATeamDeathMatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamDeathMatchGameState, RedTeam);
	DOREPLIFETIME(ATeamDeathMatchGameState, RedTeamScore);
	DOREPLIFETIME(ATeamDeathMatchGameState, BlueTeam);
	DOREPLIFETIME(ATeamDeathMatchGameState, BlueTeamScore);
}

void ATeamDeathMatchGameState::OnRep_RedTeam()
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

void ATeamDeathMatchGameState::OnRep_RedTeamScore()
{
	
}

void ATeamDeathMatchGameState::OnRep_BlueTeam()
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

void ATeamDeathMatchGameState::OnRep_BlueTeamScore()
{
}
