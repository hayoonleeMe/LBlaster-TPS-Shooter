// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterPlayerController.h"

#include "Character/LBlasterCharacter.h"
#include "HUD/LBlasterHUD.h"

void ALBlasterPlayerController::SetHUDHealth(float InHealth, float InMaxHealth)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDHealth(InHealth, InMaxHealth);
	}
}

void ALBlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Player Controller가 초기화될 때 Health HUD 업데이트
	if (ALBlasterCharacter* PlayerCharacter = Cast<ALBlasterCharacter>(InPawn))
	{
		PlayerCharacter->UpdateHUDHealth();
	}
}

bool ALBlasterPlayerController::IsValidHUD()
{
	if (!LBlasterHUD)
	{
		LBlasterHUD = Cast<ALBlasterHUD>(GetHUD());
	}
	return LBlasterHUD != nullptr;
}
