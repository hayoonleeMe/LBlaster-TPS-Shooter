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

	if (IsValidOwnerLBController())
	{
		OwnerLBController->EnablePauseMenuMappingContext();
		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(TakeWidget());
		OwnerLBController->SetInputMode(InputModeData);
		OwnerLBController->SetShowMouseCursor(true);
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
}

void UPauseMenu::MenuTearDown()
{
	Super::MenuTearDown();
	
	if (IsValidOwnerLBController())
	{
		OwnerLBController->DisablePauseMenuMappingContext();
		OwnerLBController->SetInputMode(FInputModeGameOnly());
		OwnerLBController->SetShowMouseCursor(false);
	}
}

void UPauseMenu::OnDestroySessionComplete(bool bWasSuccessful)
{
	// DestroySession에 실패하면 다시 ReturnButton을 활성화하고 리턴한다.
	if (!bWasSuccessful)
	{
		ReturnToMainMenuButton->SetIsEnabled(true);
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
			if (IsValidOwnerLBController())
			{
				OwnerLBController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

void UPauseMenu::DestroyAllClientSession()
{
	// 모든 클라이언트의 플레이어 컨트롤러에서 Client RPC를 호출해 DestroySession을 호출하게 한다.
	if (IsValidOwnerLBController())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PlayerController = It->Get(); PlayerController != OwnerLBController)
			{
				if (ASessionHelperPlayerController* SHController = Cast<ASessionHelperPlayerController>(PlayerController))
				{
					SHController->ClientDestroySession();
				}
			}
		}	
	}
}

void UPauseMenu::OnPlayerLeftGame()
{
	if (MultiplayerSessionsSubsystem)
	{
		if (IsValidOwnerLBController())
		{
			if (OwnerLBController->HasAuthority())
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

void UPauseMenu::ReturnToMainMenuButtonClicked()
{
	ReturnToMainMenuButton->SetIsEnabled(false);

	if (IsValidOwnerLBController())
	{
		if (OwnerLBController->HasAuthority())
		{
			// 호스트가 메인 메뉴로 나갈 땐 바로 세션 파괴하고 게임 종료
			OnPlayerLeftGame();
		}
		else
		{
			// 클라이언트 유저의 캐릭터를 Kill하고 메인 메뉴로 보냄.
			if (ALBlasterCharacter* LBCharacter = Cast<ALBlasterCharacter>(OwnerLBController->GetCharacter()))
			{
				LBCharacter->OnLeftGame.AddUObject(this, &ThisClass::OnPlayerLeftGame);
				LBCharacter->ServerLeaveGame();
			}
		}
	}
	
	// 다시 시도할 수 있게 버튼 활성화
    ReturnToMainMenuButton->SetIsEnabled(true);
}

void UPauseMenu::ResumeButtonClicked()
{
	MenuTearDown();
}

void UPauseMenu::SettingButtonClicked()
{
	if (IsValidLBlasterHUD())
	{
		LBlasterHUD->CreateSettingMenu();
	}
}
