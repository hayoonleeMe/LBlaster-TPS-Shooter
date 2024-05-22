// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScoreboardRow.h"
#include "ScoreboardRowWithRank.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UScoreboardRowWithRank : public UScoreboardRow
{
	GENERATED_BODY()

public:
	void SetScoreboardRowText(int32 InRank, const FString& InPlayerName, int32 InScore, int32 InDeath);
	virtual void EmptyScoreboardRowText() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RankText;
};
