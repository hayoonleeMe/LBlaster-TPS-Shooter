// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PauseMenu.h"

#include "../../../Plugins/MultiplayerSessions/Source/MultiplayerSessions/Public/MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"
#include "Player/LBlasterPlayerController.h"

void UPauseMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	if (IsValidPlayerController())
	{
		PlayerController->EnablePauseMenuMappingContext();
		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(TakeWidget());
		PlayerController->SetInputMode(InputModeData);
		PlayerController->SetShowMouseCursor(true);
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
}

void UPauseMenu::MenuTearDown()
{
	RemoveFromParent();

	if (IsValidPlayerController())
	{
		PlayerController->DisablePauseMenuMappingContext();
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
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
			if (IsValidPlayerController())
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

bool UPauseMenu::IsValidPlayerController()
{
	if (!PlayerController && GetWorld())
	{
		PlayerController = GetWorld()->GetFirstPlayerController<ALBlasterPlayerController>();
	}
	return PlayerController != nullptr;
}

void UPauseMenu::MainMenuButtonClicked()
{
	MainMenuButton->SetIsEnabled(false);

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void UPauseMenu::ResumeButtonClicked()
{
	MenuTearDown();
}
