// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterGameMode.h"

#include "Character/LBlasterCharacter.h"
#include "HUD/LBlasterHUD.h"
#include "Player/LBlasterPlayerController.h"

ALBlasterGameMode::ALBlasterGameMode()
{
	static ConstructorHelpers::FClassFinder<ALBlasterCharacter> LBlasterCharacterClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Actors/Manny/BP_LBlasterCharacter.BP_LBlasterCharacter_C'"));
	if (LBlasterCharacterClassRef.Class)
	{
		DefaultPawnClass = LBlasterCharacterClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<ALBlasterHUD> LBlasterHUDClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/UI/HUD/BP_LBlasterHUD.BP_LBlasterHUD_C'"));
	if (LBlasterHUDClassRef.Class)
	{
		HUDClass = LBlasterHUDClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<ALBlasterPlayerController> PlayerControllerClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/Controllers/Player/BP_LBlasterPlayerController.BP_LBlasterPlayerController_C'"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}
