// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamDeathMatchGameState.h"

#include "Algo/StableSort.h"
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
	Algo::StableSort(RedTeam, [](const ALBlasterPlayerState* A, const ALBlasterPlayerState* B)
	{
		const float A_Kda = A->GetDeath() != 0 ? A->GetKillScore() / static_cast<float>(A->GetDeath()) : A->GetKillScore();
		const float B_Kda = B->GetDeath() != 0 ? B->GetKillScore() / static_cast<float>(B->GetDeath()) : B->GetKillScore();
		if (FMath::IsNearlyEqual(A_Kda, B_Kda))
		{
			// KDA, 킬 수가 같으면 Death 오름차순
			if (A->GetKillScore() == B->GetKillScore())
			{
				return A->GetDeath() < B->GetDeath();
			}
			return A->GetKillScore() > B->GetKillScore();
		}
		// KDA 내림차순
		return A_Kda > B_Kda;
	});
}

void ATeamDeathMatchGameState::SortBlueTeamByKda()
{
	Algo::StableSort(BlueTeam, [](const ALBlasterPlayerState* A, const ALBlasterPlayerState* B)
	{
		const float A_Kda = A->GetDeath() != 0 ? A->GetKillScore() / static_cast<float>(A->GetDeath()) : A->GetKillScore();
		const float B_Kda = B->GetDeath() != 0 ? B->GetKillScore() / static_cast<float>(B->GetDeath()) : B->GetKillScore();
		if (FMath::IsNearlyEqual(A_Kda, B_Kda))
		{
			// KDA, 킬 수가 같으면 Death 오름차순
			if (A->GetKillScore() == B->GetKillScore())
			{
				return A->GetDeath() < B->GetDeath();
			}
			return A->GetKillScore() > B->GetKillScore();
		}
		// KDA 내림차순
		return A_Kda > B_Kda;
	});
}

void ATeamDeathMatchGameState::IncreaseRedTeamScore()
{
	SetRedTeamScore(RedTeamScore + 1, true);
}

void ATeamDeathMatchGameState::IncreaseBlueTeamScore()
{
	SetBlueTeamScore(BlueTeamScore + 1, true);
}

void ATeamDeathMatchGameState::SetRedTeamScore(const int32 InScore, bool bUpdateHUD)
{
	RedTeamScore = FMath::Min(GoalKillScore, InScore);

	// End Game
	if (RedTeamScore == GoalKillScore)
	{
		EndGameOnGoalKills();
	}

	// Update HUD
	if (bUpdateHUD && GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateTeamScoreMiniScoreboard(ETeam::ET_RedTeam, RedTeamScore);
			}
		}
	}
}

void ATeamDeathMatchGameState::SetBlueTeamScore(const int32 InScore, bool bUpdateHUD)
{
	BlueTeamScore = FMath::Min(GoalKillScore, InScore);

	// End Game
	if (BlueTeamScore == GoalKillScore)
	{
		EndGameOnGoalKills();
	}
	
	// Update HUD
	if (bUpdateHUD && GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateTeamScoreMiniScoreboard(ETeam::ET_BlueTeam, BlueTeamScore);
			}
		}
	}
}

void ATeamDeathMatchGameState::MulticastInitTeamScore_Implementation()
{
	SetRedTeamScore(0, true);
	SetBlueTeamScore(0, true);
}

void ATeamDeathMatchGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		SetRedTeamScore(RedTeamScore, true);
    	SetBlueTeamScore(BlueTeamScore, true);	
	}
}

void ATeamDeathMatchGameState::OnRep_RedTeam()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateScoreboard();
			}
		}
	}
}

void ATeamDeathMatchGameState::OnRep_RedTeamScore()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateTeamScoreMiniScoreboard(ETeam::ET_RedTeam, RedTeamScore);
			}
		}
	}
}

void ATeamDeathMatchGameState::OnRep_BlueTeam()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateScoreboard();
			}
		}
	}
}

void ATeamDeathMatchGameState::OnRep_BlueTeamScore()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateTeamScoreMiniScoreboard(ETeam::ET_BlueTeam, BlueTeamScore);			
			}
		}
	}
}
