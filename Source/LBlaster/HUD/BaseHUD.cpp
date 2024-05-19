// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BaseHUD.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

void ABaseHUD::CreateSettingMenu()
{
}

void ABaseHUD::CreateGraphicSettingMenu()
{
}

void ABaseHUD::ReturnMenu(bool bForceReturn)
{
}

void ABaseHUD::AddNewMenuToStack(ULBlasterUserWidget* InNewMenu)
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
				int32 Value;
				NamedOnlineSession->SessionSettings.Get(UMultiplayerSessionsSubsystem::MatchModeKey, Value);
				MatchModeType = static_cast<EMatchMode>(Value);
			}
		}
	}
}
