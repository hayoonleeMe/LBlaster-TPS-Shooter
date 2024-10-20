﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeForAllGameState.h"

#include "Algo/StableSort.h"
#include "HUD/LBlasterHUD.h"
#include "Net/UnrealNetwork.h"
#include "Player/LBlasterPlayerState.h"

void AFreeForAllGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFreeForAllGameState, TotalScore);
	DOREPLIFETIME(AFreeForAllGameState, LBPlayerArray);
}

void AFreeForAllGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (!HasAuthority())
	{
		return;
	}
		
	if (ALBlasterPlayerState* LBPlayerState = Cast<ALBlasterPlayerState>(PlayerState))
	{
		if (!LBPlayerState->IsInactive())
		{
			// make sure no duplicates
			LBPlayerArray.AddUnique(LBPlayerState);
		}	
	}

	UpdateScoreboard();
}

void AFreeForAllGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	if (!HasAuthority())
	{
		return;
	}
	
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

	UpdateScoreboard();
}

void AFreeForAllGameState::RemoveAllPlayerStateByName(APlayerState* PlayerState)
{
	for (int32 Index = 0; Index < LBPlayerArray.Num(); Index++)
	{
		if (LBPlayerArray[Index] && PlayerState)
		{
			if (LBPlayerArray[Index] == PlayerState || LBPlayerArray[Index]->GetPlayerName() == PlayerState->GetPlayerName())
			{
				LBPlayerArray.RemoveAt(Index, 1);
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

void AFreeForAllGameState::OnRep_LBPlayerArray()
{
	UpdateScoreboard();
}

void AFreeForAllGameState::SortPlayersByKda()
{
	// KDA를 기준으로 내림차순 정렬
	Algo::StableSort(LBPlayerArray, [](const ALBlasterPlayerState* A, const ALBlasterPlayerState* B) 
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

void AFreeForAllGameState::UpdateScoreboard() const
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
