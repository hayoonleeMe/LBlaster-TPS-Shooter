// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SettingsHubMenu.h"

#include "LBlasterHUD.h"
#include "Components/Button.h"

void USettingsHubMenu::MenuSetup()
{
	Super::MenuSetup();
	
	/* Video Setting */
	if (VideoSettingButton && !VideoSettingButton->OnClicked.IsBound())
	{
		VideoSettingButton->OnClicked.AddDynamic(this, &ThisClass::VideoSettingButtonClicked);
	}

	/* Return Button */
	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::ReturnButtonClicked);
	}
}

void USettingsHubMenu::VideoSettingButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->CreateVideoSettingMenu();
	}
}

void USettingsHubMenu::ReturnButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu();
	}
}
