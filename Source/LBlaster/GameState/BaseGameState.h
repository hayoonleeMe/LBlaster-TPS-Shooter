// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BaseGameState.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ABaseGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitGoalScoreFromSession();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_GoalScore)
	int32 GoalScore;

	UFUNCTION()
	void OnRep_GoalScore();

	void UpdateHUDGoalScore();
};
