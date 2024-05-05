// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LBlasterUserWidget.h"

#include "LBlasterHUD.h"
#include "Player/LBlasterPlayerController.h"

bool ULBlasterUserWidget::IsValidOwnerLBController()
{
	if (!OwnerLBController && GetOwningPlayer())
	{
		OwnerLBController = Cast<ALBlasterPlayerController>(GetOwningPlayer());
	}
	return OwnerLBController != nullptr;
}

bool ULBlasterUserWidget::IsValidLBlasterHUD()
{
	if (!LBlasterHUD && IsValidOwnerLBController() && OwnerLBController->GetHUD())
	{
		LBlasterHUD = Cast<ALBlasterHUD>(OwnerLBController->GetHUD());
	}
	return LBlasterHUD != nullptr;
}
