// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LBlasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ALBlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALBlasterPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Death();
	
	void AddToScore(float InScoreAmount);
	void AddToDeath(int32 InDeathAmount);
	

private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwnerCharacter;

	bool IsValidOwnerCharacter();

	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> OwnerController;
	
	bool IsValidOwnerController();

	UPROPERTY(ReplicatedUsing=OnRep_Death)
	int32 Death;
};
