// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterGameMode.h"

#include "Character/LBlasterCharacter.h"

ALBlasterGameMode::ALBlasterGameMode()
{
	static ConstructorHelpers::FClassFinder<ALBlasterCharacter> LBlasterCharacterClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Actors/Players/Manny/BP_LBlasterCharacter.BP_LBlasterCharacter_C'"));
	if (LBlasterCharacterClassRef.Class)
	{
		DefaultPawnClass = LBlasterCharacterClassRef.Class;
	}
}
