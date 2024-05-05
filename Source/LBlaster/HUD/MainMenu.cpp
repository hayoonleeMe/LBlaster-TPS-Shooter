// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Button.h"
#include "HUD/MainMenuHUD.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenu::MenuSetup()
{
	Super::MenuSetup();

	/* Input Mode */
	if (IsValidOwnerController())
	{
		// 위젯이 생성되고 메뉴 셋업이 끝나면 InputMode를 세팅한다. UIOnly이므로 다른 Pawn들 말고 UI에만 입력을 적용한다.
		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(TakeWidget());
		// 마우스 커서가 게임 화면 밖으로 나갈 수 있도록 한다. 
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		OwnerController->SetInputMode(InputModeData);
		OwnerController->SetShowMouseCursor(true);
	}
	
	/* Main Overlay */
	if (StartButton && !StartButton->OnClicked.IsBound())
	{
		StartButton->OnClicked.AddDynamic(this, &ThisClass::OnStartButtonClicked);
	}
	if (SettingButton && !SettingButton->OnClicked.IsBound())
	{
		SettingButton->OnClicked.AddDynamic(this, &ThisClass::OnSettingButtonClicked);
	}
	if (QuitButton && !QuitButton->OnClicked.IsBound())
	{
		QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnQuitButtonClicked);
	}
}

void UMainMenu::MenuTearDown()
{
	Super::MenuTearDown();

	/* Input Mode */
	if (IsValidOwnerController())
	{
		FInputModeGameOnly InputModeData;
		OwnerController->SetInputMode(InputModeData);
		OwnerController->SetShowMouseCursor(false);
	}
}

void UMainMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMainMenu::OnStartButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->CreateStartMenu();
	}
}

void UMainMenu::OnSettingButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->CreateSettingMenu();
	}
}

void UMainMenu::OnQuitButtonClicked()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
	}
}
