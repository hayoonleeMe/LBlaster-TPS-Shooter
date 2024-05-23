// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MiniScoreboard.h"
#include "MiniScoreboardFreeForAll.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UMiniScoreboardFreeForAll : public UMiniScoreboard
{
	GENERATED_BODY()

public:
	void SetTotalScoreText(const int32 InTotalScore);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TotalScoreText;
};
