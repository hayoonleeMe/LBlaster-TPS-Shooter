// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SettingMenu.h"

#include "LBlasterHUD.h"
#include "Components/Button.h"

void USettingMenu::MenuSetup()
{
	Super::MenuSetup();
	
	/* Graphic Setting */
	if (GraphicSettingButton && !GraphicSettingButton->OnClicked.IsBound())
	{
		GraphicSettingButton->OnClicked.AddDynamic(this, &ThisClass::GraphicSettingButtonClicked);
	}

	/* Return Button */
	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::ReturnButtonClicked);
	}
}

void USettingMenu::GraphicSettingButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->CreateGraphicSettingMenu();
	}
}

void USettingMenu::ReturnButtonClicked()
{
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu();
	}
}
