// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LBlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ALBlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float InScore);
	void SetHUDDeath(int32 InDeath);

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	TObjectPtr<class ALBlasterHUD> LBlasterHUD;

	bool IsValidHUD();
};
