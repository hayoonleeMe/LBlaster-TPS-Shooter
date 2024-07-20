// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameState.h"

#include "MultiplayerMapPath.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GameMode/BaseGameMode.h"
#include "HUD/LBlasterHUD.h"
#include "Net/UnrealNetwork.h"

ABaseGameState::ABaseGameState()
{
#if WITH_EDITOR
	GoalKillScore = 10;
#endif
}

void ABaseGameState::EndGameOnGoalKills()
{
	if (ABaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<ABaseGameMode>())
	{
		BaseGameMode->SetMatchStateToCooldown();
	}
}

void ABaseGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		InitGoalKillScoreFromSession();	
	}
}

void ABaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseGameState, GoalKillScore);
}

void ABaseGameState::InitGoalKillScoreFromSession()
{
#if !WITH_EDITOR
	if (GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			if (FNamedOnlineSession* NamedOnlineSession = MultiplayerSessionsSubsystem->GetNamedOnlineSession())
			{
				NamedOnlineSession->SessionSettings.Get(FMultiplayerMapPath::GoalKillScoreKey, GoalKillScore);
			}
		}
	}
#endif
	
	UpdateHUDGoalKillScore();
}

void ABaseGameState::MulticastInitGoalKillScore_Implementation()
{
	InitGoalKillScoreFromSession();
}

void ABaseGameState::OnRep_GoalKillScore()
{
	UpdateHUDGoalKillScore();
}

void ABaseGameState::UpdateHUDGoalKillScore()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ALBlasterHUD* HUD = PlayerController->GetHUD<ALBlasterHUD>())
			{
				HUD->SetGoalKillScoreMiniScoreboard(GoalKillScore);
			}
		}
	}
}
