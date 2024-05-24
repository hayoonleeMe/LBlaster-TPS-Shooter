// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterGameMode.h"
#include "FreeForAllGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AFreeForAllGameMode : public ALBlasterGameMode
{
	GENERATED_BODY()

public:
	AFreeForAllGameMode();
	virtual void Logout(AController* Exiting) override;
	virtual void PlayerEliminated(ALBlasterCharacter* EliminatedCharacter, ALBlasterPlayerController* VictimController, ALBlasterPlayerController* AttackerController) override;
};
