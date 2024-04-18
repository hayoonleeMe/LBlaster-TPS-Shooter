// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PauseMenu.h"

#include "LBlasterHUD.h"
#include "SettingMenu.h"
#include "../../../Plugins/MultiplayerSessions/Source/MultiplayerSessions/Public/MultiplayerSessionsSubsystem.h"
#include "Character/LBlasterCharacter.h"
#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"
#include "Player/LBlasterPlayerController.h"

void UPauseMenu::MenuSetup()
{
	Super::MenuSetup();

	if (IsValidOwnerController())
	{
		OwnerController->EnablePauseMenuMappingContext();
		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(TakeWidget());
		OwnerController->SetInputMode(InputModeData);
		OwnerController->SetShowMouseCursor(true);
	}

	/* Main Menu Button */
	if (MainMenuButton && !MainMenuButton->OnClicked.IsBound())
	{
		MainMenuButton->OnClicked.AddDynamic(this, &ThisClass::UPauseMenu::MainMenuButtonClicked);
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if (MultiplayerSessionsSubsystem && !MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionCompleteDelegate.IsBound())
		{
			MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySession);
		}
	}

	/* Resume Button */
	if (ResumeButton && !ResumeButton->OnClicked.IsBound())
	{
		ResumeButton->OnClicked.AddDynamic(this, &ThisClass::ResumeButtonClicked);
	}

	/* Setting Button */
	if (SettingButton && !SettingButton->OnClicked.IsBound())
	{
		SettingButton->OnClicked.AddDynamic(this, &ThisClass::SettingButtonClicked);
	}
}

void UPauseMenu::MenuTearDown()
{
	Super::MenuTearDown();
	
	if (IsValidOwnerController())
	{
		OwnerController->DisablePauseMenuMappingContext();
		OwnerController->SetInputMode(FInputModeGameOnly());
		OwnerController->SetShowMouseCursor(false);
	}
}

void UPauseMenu::OnDestroySession(bool bWasSuccessful)
{
	// DestroySession에 실패하면 다시 ReturnButton을 활성화하고 리턴한다.
	if (!bWasSuccessful)
	{
		MainMenuButton->SetIsEnabled(true);
		return;
	}

	if (UWorld* World = GetWorld())
	{
		// Server
		if (AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>())
		{
			GameMode->ReturnToMainMenuHost();
		}
		// Client
		else
		{
			if (IsValidOwnerController())
			{
				OwnerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

void UPauseMenu::OnPlayerLeftGame()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void UPauseMenu::MainMenuButtonClicked()
{
	MainMenuButton->SetIsEnabled(false);

	if (IsValidOwnerController())
	{
		if (ALBlasterCharacter* LBCharacter = Cast<ALBlasterCharacter>(OwnerController->GetCharacter()))
		{
			LBCharacter->OnLeftGame.AddUObject(this, &ThisClass::OnPlayerLeftGame);
			LBCharacter->ServerLeaveGame();
		}
		else
		{
			// 다시 시도할 수 있게 버튼 활성화
			MainMenuButton->SetIsEnabled(true);
		}
	}
}

void UPauseMenu::ResumeButtonClicked()
{
	MenuTearDown();
}

void UPauseMenu::SettingButtonClicked()
{
	if (IsValidHUD())
	{
		LBlasterHUD->CreateSettingMenu();
	}
}
