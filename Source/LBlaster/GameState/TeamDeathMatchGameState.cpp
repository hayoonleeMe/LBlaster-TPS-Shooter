// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamDeathMatchGameState.h"

#include "HUD/LBlasterHUD.h"
#include "Net/UnrealNetwork.h"
#include "Player/LBlasterPlayerState.h"

void ATeamDeathMatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamDeathMatchGameState, RedTeam);
	DOREPLIFETIME(ATeamDeathMatchGameState, RedTeamScore);
	DOREPLIFETIME(ATeamDeathMatchGameState, BlueTeam);
	DOREPLIFETIME(ATeamDeathMatchGameState, BlueTeamScore);
}

void ATeamDeathMatchGameState::AddUniqueRedTeamPlayer(ALBlasterPlayerState* InPlayerState)
{
	RedTeam.AddUnique(InPlayerState);
}

void ATeamDeathMatchGameState::AddUniqueBlueTeamPlayer(ALBlasterPlayerState* InPlayerState)
{
	BlueTeam.AddUnique(InPlayerState);
}

void ATeamDeathMatchGameState::RemoveRedTeamPlayer(ALBlasterPlayerState* InPlayerState)
{
	RedTeam.Remove(InPlayerState);
}

void ATeamDeathMatchGameState::RemoveBlueTeamPlayer(ALBlasterPlayerState* InPlayerState)
{
	BlueTeam.Remove(InPlayerState);
}

void ATeamDeathMatchGameState::SortRedTeamByKda()
{
	Algo::Sort(RedTeam, [](const ALBlasterPlayerState* A, const ALBlasterPlayerState* B)
	{
		const float A_KDA = A->GetDeath() != 0 ? A->GetScore() / A->GetDeath() : A->GetScore();
		const float B_KDA = B->GetDeath() != 0 ? B->GetScore() / B->GetDeath() : B->GetScore();
		return A_KDA > B_KDA;
	});
}

void ATeamDeathMatchGameState::SortBlueTeamByKda()
{
	Algo::Sort(BlueTeam, [](const ALBlasterPlayerState* A, const ALBlasterPlayerState* B)
	{
		const float A_Kda = A->GetDeath() != 0 ? A->GetScore() / A->GetDeath() : A->GetScore();
		const float B_Kda = B->GetDeath() != 0 ? B->GetScore() / B->GetDeath() : B->GetScore();
		return A_Kda > B_Kda;
	});
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
