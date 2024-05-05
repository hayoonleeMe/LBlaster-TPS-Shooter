// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GraphicSettingMenuForMainMenu.h"

#include "MainMenuHUD.h"
#include "Components/Button.h"
#include "Components/Overlay.h"

void UGraphicSettingMenuForMainMenu::MenuSetup()
{
	Super::MenuSetup();

	/* Return Button */
	// AMainMenuHUD로 돌아가도록 변경
	if (ReturnButton)
	{
		ReturnButton->OnClicked.Clear();
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::MainMenuReturnButtonClicked);
	}
}

void UGraphicSettingMenuForMainMenu::MainMenuReturnButtonClicked()
{
	if (ShouldApplyChange())
	{
		// 변경사항 있다고 alert
		if (NoApplyAlertOverlay)
		{
			NoApplyAlertOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (IsValidOwnerHUD())
		{
			OwnerHUD->ReturnMenu();
		}	
	}
}
