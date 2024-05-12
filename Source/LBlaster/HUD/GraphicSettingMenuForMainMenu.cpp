// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GraphicSettingMenuForMainMenu.h"

#include "MainMenuHUD.h"
#include "Components/Overlay.h"

void UGraphicSettingMenuForMainMenu::CloseOverlay()
{
	if (NoApplyAlertOverlay && NoApplyAlertOverlay->IsVisible())
	{
		OnNoApplyAlertCancelButtonClicked();
	}
	else
	{
		OnReturnButtonClicked();
	}
}

void UGraphicSettingMenuForMainMenu::OnReturnButtonClicked()
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
		// Main Menu로 돌아감
		if (IsValidOwnerHUD())
		{
			OwnerHUD->ReturnMenu();
		}	
	}
}

void UGraphicSettingMenuForMainMenu::OnNoApplyAlertAcceptButtonClicked()
{
	Super::OnNoApplyAlertAcceptButtonClicked();

	// Main Menu로 돌아감
	if (IsValidOwnerHUD())
	{
		OwnerHUD->ReturnMenu(true);
	}	
}
