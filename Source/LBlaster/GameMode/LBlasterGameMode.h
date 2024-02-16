// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LBlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ALBlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ALBlasterGameMode();

	virtual void PlayerEliminated(class ALBlasterCharacter* EliminatedCharacter, class ALBlasterPlayerController* VictimController, ALBlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);
};
