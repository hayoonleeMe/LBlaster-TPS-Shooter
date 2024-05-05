// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SettingMenuForMainMenu.h"

#include "MainMenuHUD.h"
#include "Components/Button.h"

void USettingMenuForMainMenu::MenuSetup()
{
	UMainMenuUserWidget::MenuSetup();

	/* Graphic Setting */
	if (GraphicSettingButton && !GraphicSettingButton->OnClicked.IsBound())
	{
		GraphicSettingButton->OnClicked.AddDynamic(this, &ThisClass::MainMenuGraphicSettingButtonClicked);
	}

	/* Return Button */
	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::MainMenuReturnButtonClicked);
	}
}

void USettingMenuForMainMenu::MainMenuGraphicSettingButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->CreateGraphicSettingMenu();
	}
}

void USettingMenuForMainMenu::MainMenuReturnButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu();
	}
}
