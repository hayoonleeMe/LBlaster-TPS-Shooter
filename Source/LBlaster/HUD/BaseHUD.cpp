// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BaseHUD.h"

#include "LBlaster.h"
#include "MultiplayerMapPath.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GameMode/TeamDeathMatchGameMode.h"
#include "GameState/FreeForAllGameState.h"
#include "Kismet/GameplayStatics.h"

void ABaseHUD::CreateSettingMenu()
{
}

void ABaseHUD::CreateVideoSettingMenu()
{
}

void ABaseHUD::CreateMouseSettingMenu()
{
}

void ABaseHUD::CreateAudioSettingMenu()
{
}

void ABaseHUD::ReturnMenu(bool bForceReturn)
{
}

void ABaseHUD::AddNewMenuToStack(ULBlasterUserWidget* InNewMenu)
{
}

void ABaseHUD::AddChatMessage(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam)
{
}

void ABaseHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			if (FNamedOnlineSession* NamedOnlineSession = MultiplayerSessionsSubsystem->GetNamedOnlineSession())
			{
				/* Match Mode */
				int32 Value;
				NamedOnlineSession->SessionSettings.Get(FMultiplayerMapPath::MatchModeKey, Value);
				MatchModeType = static_cast<EMatchMode>(Value);

				/* Goal Kill Score */
				NamedOnlineSession->SessionSettings.Get(FMultiplayerMapPath::GoalKillScoreKey, GoalKillScore);
			}
		}
	}
}

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	// set MatchModeType for debugging in editor
#if WITH_EDITOR
	MatchModeType = (UGameplayStatics::GetActorOfClass(GetWorld(), AFreeForAllGameState::StaticClass()) != nullptr) ? EMatchMode::FreeForAll : EMatchMode::TeamDeathMatch;
#endif
}
