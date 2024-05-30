// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PauseMenu.h"

#include "LBlasterHUD.h"
#include "SettingMenu.h"
#include "../../../Plugins/MultiplayerSessions/Source/MultiplayerSessions/Public/MultiplayerSessionsSubsystem.h"
#include "Character/LBlasterCharacter.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "GameFramework/GameModeBase.h"
#include "Player/LBlasterPlayerController.h"

void UPauseMenu::MenuSetup()
{
	Super::MenuSetup();

	if (IsValidOwnerController())
	{
		if (ALBlasterPlayerController* LBOwnerController = Cast<ALBlasterPlayerController>(OwnerController))
		{
			LBOwnerController->EnablePauseMenuMappingContext();
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			LBOwnerController->SetInputMode(InputModeData);
			LBOwnerController->SetShowMouseCursor(true);
		}
	}

	/* Main Menu Button */
	if (ReturnToMainMenuButton && !ReturnToMainMenuButton->OnClicked.IsBound())
	{
		ReturnToMainMenuButton->OnClicked.AddDynamic(this, &ThisClass::UPauseMenu::ReturnToMainMenuButtonClicked);
	}

	/* Multiplayer Sessions */
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->LBOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);
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

	/* Return To Main Menu Alert Overlay */
	if (ReturnToMainMenuAlertAcceptButton && !ReturnToMainMenuAlertAcceptButton->OnClicked.IsBound())
	{
		ReturnToMainMenuAlertAcceptButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnToMainMenuAlertAcceptButtonClicked);
	}
	if (ReturnToMainMenuAlertCancelButton && !ReturnToMainMenuAlertCancelButton->OnClicked.IsBound())
	{
		ReturnToMainMenuAlertCancelButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnToMainMenuAlertCancelButtonClicked);
	}
}

void UPauseMenu::MenuTearDown()
{
	Super::MenuTearDown();
	
	if (IsValidOwnerController())
	{
		if (ALBlasterPlayerController* LBOwnerController = Cast<ALBlasterPlayerController>(OwnerController))
		{
			LBOwnerController->DisablePauseMenuMappingContext();
			LBOwnerController->SetInputMode(FInputModeGameOnly());
			LBOwnerController->SetShowMouseCursor(false);	
		}
	}
}

void UPauseMenu::OnDestroySessionComplete(bool bWasSuccessful)
{
	SetLoadingOverlayVisibility(false);

	// 실패하면 다시 수행.
	if (!bWasSuccessful)
	{
		OnReturnToMainMenuAlertAcceptButtonClicked();
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

void UPauseMenu::DestroyAllClientSession()
{
	// 모든 클라이언트의 플레이어 컨트롤러에서 Client RPC를 호출해 DestroySession을 호출하게 한다.
	if (IsValidOwnerController())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PlayerController = It->Get(); PlayerController != OwnerController)
			{
				if (ABasePlayerController* SHController = Cast<ABasePlayerController>(PlayerController))
				{
					SHController->ClientDestroySession();
				}
			}
		}	
	}
}

void UPauseMenu::ReturnToMainMenuButtonClicked()
{
	if (ReturnToMainMenuAlertOverlay)
	{
		ReturnToMainMenuAlertOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPauseMenu::ResumeButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		if (ALBlasterHUD* HUD = Cast<ALBlasterHUD>(OwnerHUD))
		{
			HUD->ShowPauseMenu();
		}
	}
}

void UPauseMenu::SettingButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->CreateSettingMenu();
	}
}

void UPauseMenu::OnReturnToMainMenuAlertAcceptButtonClicked()
{
	SetLoadingOverlayVisibility(true);

	if (IsValidOwnerController() && MultiplayerSessionsSubsystem)
	{
		if (ALBlasterPlayerController* LBOwnerController = Cast<ALBlasterPlayerController>(OwnerController))
		{
			// 유저 캐릭터를 제거하고 메인 메뉴로 보냄.
			LBOwnerController->ServerLeaveGame();

			if (LBOwnerController->HasAuthority())
			{
				DestroyAllClientSession();
				MultiplayerSessionsSubsystem->DestroySession();
			}
			else
			{
				MultiplayerSessionsSubsystem->DestroySession();
			}
		}
	}
}

void UPauseMenu::OnReturnToMainMenuAlertCancelButtonClicked()
{
	if (ReturnToMainMenuAlertOverlay)
	{
		ReturnToMainMenuAlertOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPauseMenu::SetLoadingOverlayVisibility(bool bShow)
{
	if (LoadingOverlay)
	{
		LoadingOverlay->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
