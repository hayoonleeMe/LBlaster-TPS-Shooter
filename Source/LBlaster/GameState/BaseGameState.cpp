// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameState.h"

#include "MultiplayerMapPath.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "HUD/LBlasterHUD.h"
#include "Net/UnrealNetwork.h"

void ABaseGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		InitGoalScoreFromSession();	
	}
}

void ABaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseGameState, GoalScore);
}

void ABaseGameState::InitGoalScoreFromSession()
{
	if (GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			if (FNamedOnlineSession* NamedOnlineSession = MultiplayerSessionsSubsystem->GetNamedOnlineSession())
			{
				NamedOnlineSession->SessionSettings.Get(FMultiplayerMapPath::GoalScoreKey, GoalScore);
			}
		}
	}
	
	UpdateHUDGoalScore();
}

void ABaseGameState::OnRep_GoalScore()
{
	UpdateHUDGoalScore();
}

void ABaseGameState::UpdateHUDGoalScore()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->SetGoalScoreMiniScoreboard(GoalScore);
			}
		}
	}
}
