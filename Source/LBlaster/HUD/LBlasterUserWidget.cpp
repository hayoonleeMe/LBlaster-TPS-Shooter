// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LBlasterUserWidget.h"

#include "LBlasterHUD.h"
#include "Player/LBlasterPlayerController.h"

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

bool ULBlasterUserWidget::IsValidOwnerController()
{
	if (!OwnerController && GetOwningPlayer())
	{
		OwnerController = Cast<ALBlasterPlayerController>(GetOwningPlayer());
	}
	return OwnerController != nullptr;
}

bool ULBlasterUserWidget::IsValidHUD()
{
	if (!LBlasterHUD && IsValidOwnerController() && OwnerController->GetHUD())
	{
		LBlasterHUD = Cast<ALBlasterHUD>(OwnerController->GetHUD());
	}
	return LBlasterHUD != nullptr;
}
