// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeForAllGameState.h"

#include "Algo/StableSort.h"
#include "HUD/LBlasterHUD.h"
#include "Net/UnrealNetwork.h"
#include "Player/LBlasterPlayerState.h"

void AFreeForAllGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFreeForAllGameState, TotalScore);
}

void AFreeForAllGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (ALBlasterPlayerState* LBPlayerState = Cast<ALBlasterPlayerState>(PlayerState))
	{
		if (!LBPlayerState->IsInactive())
		{
			// make sure no duplicates
			LBPlayerArray.AddUnique(LBPlayerState);
		}	
	}
	
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

void AFreeForAllGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	if (ALBlasterPlayerState* LBPlayerState = Cast<ALBlasterPlayerState>(PlayerState))
	{
		for (int32 Index = 0; Index < LBPlayerArray.Num(); Index++)
		{
			if (LBPlayerArray[Index] == LBPlayerState)
			{
				LBPlayerArray.RemoveAt(Index, 1);
				return;
			}
		}	
	}

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

void AFreeForAllGameState::IncreaseTotalScore()
{
	SetTotalScore(TotalScore + 1, true);
}

void AFreeForAllGameState::SetTotalScore(const int32 InScore, bool bUpdateHUD)
{
	TotalScore = FMath::Min(GoalKillScore, InScore);

	// End Game
	if (TotalScore == GoalKillScore)
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
				HUD->UpdateTotalScoreMiniScoreboard(TotalScore);
			}
		}
	}
}

void AFreeForAllGameState::MulticastInitTotalScore_Implementation()
{
	SetTotalScore(0, true);
}

void AFreeForAllGameState::SortPlayersByKda()
{
	// KDA를 기준으로 내림차순 정렬
	Algo::StableSort(LBPlayerArray, [](const ALBlasterPlayerState* A, const ALBlasterPlayerState* B) 
	{
		const float A_Kda = A->GetDeath() != 0 ? A->GetKillScore() / A->GetDeath() : A->GetKillScore();
		const float B_Kda = B->GetDeath() != 0 ? B->GetKillScore() / B->GetDeath() : B->GetKillScore();
		return A_Kda > B_Kda;
	});
}

void AFreeForAllGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		SetTotalScore(TotalScore, true);
	}
}

void AFreeForAllGameState::OnRep_TotalScore()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->UpdateTotalScoreMiniScoreboard(TotalScore);
			}
		}
	}
}
