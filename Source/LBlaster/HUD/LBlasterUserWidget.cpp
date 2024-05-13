// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LBlasterUserWidget.h"

#include "BaseHUD.h"

void ULBlasterUserWidget::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
}

void ULBlasterUserWidget::MenuTearDown()
{
	RemoveFromParent();
}

bool ULBlasterUserWidget::IsOverlayOpened()
{
	return false;
}

void ULBlasterUserWidget::CloseOverlay()
{
}

bool ULBlasterUserWidget::CanReturn()
{
	return true;
}

bool ULBlasterUserWidget::IsValidOwnerController()
{
	if (!OwnerController && GetOwningPlayer())
	{
		OwnerController = GetOwningPlayer();
	}
	return OwnerController != nullptr;
}

bool ULBlasterUserWidget::IsValidOwnerHUD()
{
	if (!OwnerHUD && IsValidOwnerController() && OwnerController->GetHUD())
	{
		OwnerHUD = Cast<ABaseHUD>(OwnerController->GetHUD());
	}
	return OwnerHUD != nullptr;
}
