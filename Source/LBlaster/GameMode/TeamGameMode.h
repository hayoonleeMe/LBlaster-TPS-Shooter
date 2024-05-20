// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterGameMode.h"
#include "TeamGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ATeamGameMode : public ALBlasterGameMode
{
	GENERATED_BODY()

public:
	ATeamGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
};
