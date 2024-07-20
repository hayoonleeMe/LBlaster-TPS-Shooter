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
	ABaseGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastInitGoalKillScore();

	void EndGameOnGoalKills();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_GoalKillScore)
	int32 GoalKillScore;

	UFUNCTION()
	void OnRep_GoalKillScore();

	void InitGoalKillScoreFromSession();
	void UpdateHUDGoalKillScore();
};
