// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MiniScoreboard.h"
#include "MiniScoreboardTeamDeathMatch.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UMiniScoreboardTeamDeathMatch : public UMiniScoreboard
{
	GENERATED_BODY()

public:
	void SetRedTeamScoreText(const int32 InTeamScore);
	void SetBlueTeamScoreText(const int32 InTeamScore);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RedTeamScoreText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BlueTeamScoreText;
};
