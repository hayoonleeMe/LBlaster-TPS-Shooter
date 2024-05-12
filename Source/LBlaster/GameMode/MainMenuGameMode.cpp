// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/MainMenuGameMode.h"

#include "HUD/MainMenuHUD.h"
#include "Player/MainMenuPlayerController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	/* Default Class */
	static ConstructorHelpers::FClassFinder<AMainMenuHUD> MainMenuHUDClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/HUD/BP_MainMenuHUD.BP_MainMenuHUD_C'"));
	if (MainMenuHUDClassRef.Class)
	{
		HUDClass = MainMenuHUDClassRef.Class;
	}

	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
}
