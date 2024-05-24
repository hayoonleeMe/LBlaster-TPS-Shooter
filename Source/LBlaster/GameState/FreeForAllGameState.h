// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "FreeForAllGameState.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AFreeForAllGameState : public ABaseGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	void IncreaseTotalScore();
	void SetTotalScore(const int32 InScore, bool bUpdateHUD);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastInitTotalScore();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(ReplicatedUsing=OnRep_TotalScore)
	int32 TotalScore;

	UFUNCTION()
	void OnRep_TotalScore();
};
