// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Scoreboard.h"
#include "ScoreboardTeamDeathMatch.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UScoreboardTeamDeathMatch : public UScoreboard
{
	GENERATED_BODY()

public:
	virtual void UpdateBoard() override;
};
