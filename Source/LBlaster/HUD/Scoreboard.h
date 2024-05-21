// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "Scoreboard.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UScoreboard : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void UpdateBoard();

private:
	UPROPERTY(EditAnywhere, Category="LBlaster")
	TSubclassOf<class UScoreboardRow> ScoreboardRowClass;
};
