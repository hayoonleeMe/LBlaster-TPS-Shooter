// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeForAllGameState.h"

#include "HUD/LBlasterHUD.h"
#include "Net/UnrealNetwork.h"

void AFreeForAllGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFreeForAllGameState, TotalScore);
}

void AFreeForAllGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

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
	TotalScore = InScore;

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
