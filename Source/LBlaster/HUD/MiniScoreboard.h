// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "MiniScoreboard.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UMiniScoreboard : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	void SetGoalKillScoreText(const int32 InGoalKillScore);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> GoalKillScoreText;
};
