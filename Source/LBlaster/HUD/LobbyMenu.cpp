// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LobbyMenu.h"

#include "ChatBox.h"
#include "LobbyHUD.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/LBlasterPlayerState.h"

void ULobbyMenu::MenuSetup()
{
	Super::MenuSetup();

	/* Input Mode */
	if (IsValidOwnerController())
	{
		OwnerController->SetInputMode(FInputModeGameAndUI());
		OwnerController->SetShowMouseCursor(true);
	}

	/* Menu Button */
	if (IsValidOwnerController())
	{
		if (OwnerController->HasAuthority())
		{
			if (ServerBox)
			{
				ServerBox->SetVisibility(ESlateVisibility::Visible);
			}
			if (ReturnAlertTextBlock)
			{
				ReturnAlertTextBlock->SetText(FText::FromString(ServerReturnAlertText));
			}
		}
		else
		{
			if (ClientBox)
			{
				ClientBox->SetVisibility(ESlateVisibility::Visible);
			}
			if (ReturnAlertTextBlock)
			{
				ReturnAlertTextBlock->SetText(FText::FromString(ClientReturnAlertText));
			}
		}
	}

	/* Players Text */
	SetNumPlayersText(0, 0);

	/* Widgets */
	if (StartButton && !StartButton->OnClicked.IsBound())
	{
		StartButton->OnClicked.AddDynamic(this, &ThisClass::OnStartButtonClicked);
	}
	if (ServerReturnButton && !ServerReturnButton->OnClicked.IsBound())
	{
		ServerReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}
	if (ClientReturnButton && !ClientReturnButton->OnClicked.IsBound())
	{
		ClientReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}

	/* Return Alert Overlay */
	if (ReturnAlertAcceptButton && !ReturnAlertAcceptButton->OnClicked.IsBound())
	{
		ReturnAlertAcceptButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
	}
	if (ReturnAlertCancelButton && !ReturnAlertCancelButton->OnClicked.IsBound())
	{
		ReturnAlertCancelButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnAlertCancelButtonClicked);
	}
}

void ULobbyMenu::OnDestroySessionComplete(bool bWasSuccessful)
{
	// 실패하면 다시 수행.
	if (!bWasSuccessful)
	{
		OnReturnButtonClicked();
	}
}

void ULobbyMenu::OnStartButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		if (ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(OwnerHUD))
		{
			LobbyHUD->TravelToMatch();
		}
	}
}

void ULobbyMenu::OnReturnAlertCancelButtonClicked()
{
	if (ReturnAlertOverlay)
	{
		ReturnAlertOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULobbyMenu::OnReturnButtonClicked()
{
	if (ReturnAlertOverlay)
	{
		if (ReturnAlertOverlay->IsVisible())
		{
			if (ReturnAlertOverlay)
			{
				ReturnAlertOverlay->SetVisibility(ESlateVisibility::Collapsed);
			}
		
			if (IsValidOwnerHUD())
			{
				if (ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(OwnerHUD))
				{
					LobbyHUD->ReturnToMainMenu();
				}
			}
		}
		else
		{
			ReturnAlertOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ULobbyMenu::SetNumPlayersText()
{
	if (NumPlayersText)
	{
		FString Str = FString::Printf(TEXT("%d / %d"), NumCurrentPlayers, NumMaxPlayers);
		NumPlayersText->SetText(FText::FromString(Str));
	}
}

void ULobbyMenu::SetNumPlayersText(int32 InNumCurrentPlayers, int32 InNumMaxPlayers)
{
	NumCurrentPlayers = InNumCurrentPlayers;
	NumMaxPlayers = InNumMaxPlayers;
	SetNumPlayersText();
}

void ULobbyMenu::AddNumCurrentPlayersText(int32 AmountToAdd)
{
	NumCurrentPlayers = FMath::Clamp(NumCurrentPlayers + AmountToAdd, 0, NumMaxPlayers);
	SetNumPlayersText();
}

bool ULobbyMenu::IsValidOwnerPlayerState()
{
	if (!OwnerPlayerState && IsValidOwnerController())
	{
		OwnerPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>();
	}
	return OwnerPlayerState != nullptr;
}

void ULobbyMenu::SetNumMaxPlayersText(int32 InNumMaxPlayers)
{
	NumMaxPlayers = InNumMaxPlayers;
	SetNumPlayersText();
}

void ULobbyMenu::ReturnMenuByKeyboard()
{
	if (ReturnAlertOverlay)
	{
		if (!ReturnAlertOverlay->IsVisible())
		{
			ReturnAlertOverlay->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			OnReturnAlertCancelButtonClicked();
		}
	}
}
