// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LBlasterGameState.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ALBlasterGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	 *	Team
	 */
	UPROPERTY(ReplicatedUsing=OnRep_RedTeam)
	TArray<TObjectPtr<class ALBlasterPlayerState>> RedTeam;

	UFUNCTION()
	void OnRep_RedTeam();

	UPROPERTY(ReplicatedUsing=OnRep_RedTeamScore)
	float RedTeamScore;

	UFUNCTION()
	void OnRep_RedTeamScore();

	UPROPERTY(ReplicatedUsing=OnRep_BlueTeam)
	TArray<TObjectPtr<ALBlasterPlayerState>> BlueTeam;

	UFUNCTION()
	void OnRep_BlueTeam();

	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamScore)
	float BlueTeamScore;

	UFUNCTION()
	void OnRep_BlueTeamScore();
};
