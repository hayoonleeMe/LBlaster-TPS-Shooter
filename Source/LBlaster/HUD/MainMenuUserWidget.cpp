// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainMenuUserWidget.h"

#include "MainMenuHUD.h"

void UMainMenuUserWidget::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
}

void UMainMenuUserWidget::MenuTearDown()
{
	RemoveFromParent();
}

bool UMainMenuUserWidget::IsValidOwnerController()
{
	if (!OwnerController && GetOwningPlayer())
	{
		OwnerController = GetOwningPlayer();
	}
	return OwnerController != nullptr;
}

bool UMainMenuUserWidget::IsValidOwnerHUD()
{
	if (!OwnerHUD && IsValidOwnerController() && OwnerController->GetHUD())
	{
		OwnerHUD = Cast<AMainMenuHUD>(OwnerController->GetHUD());
	}
	return OwnerHUD != nullptr;
}
