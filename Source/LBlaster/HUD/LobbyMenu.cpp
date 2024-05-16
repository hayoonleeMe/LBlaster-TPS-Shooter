// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LobbyMenu.h"

#include "LobbyHUD.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

void ULobbyMenu::MenuSetup()
{
	Super::MenuSetup();

	/* Input Mode */
	if (IsValidOwnerController())
	{
		// 위젯이 생성되고 메뉴 셋업이 끝나면 InputMode를 세팅한다. UIOnly이므로 다른 Pawn들 말고 UI에만 입력을 적용한다.
		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(TakeWidget());
		// 마우스 커서가 게임 화면 밖으로 나갈 수 있도록 한다. 
		OwnerController->SetInputMode(InputModeData);
		OwnerController->SetShowMouseCursor(true);
	}

	SetNumPlayersText(0, 0);

	if (StartButton && !StartButton->OnClicked.IsBound())
	{
		StartButton->OnClicked.AddDynamic(this, &ThisClass::OnStartButtonClicked);
	}
	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnButtonClicked);
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

void ULobbyMenu::OnReturnButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		if (ALobbyHUD* LobbyHUD = Cast<ALobbyHUD>(OwnerHUD))
		{
			LobbyHUD->ReturnToMainMenu();
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

void ULobbyMenu::SetNumMaxPlayersText(int32 InNumMaxPlayers)
{
	NumMaxPlayers = InNumMaxPlayers;
	SetNumPlayersText();
}
