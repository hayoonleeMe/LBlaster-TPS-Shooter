// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "BasePlayerStart.generated.h"

UCLASS()
class LBLASTER_API ABasePlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	ABasePlayerStart(const FObjectInitializer& ObjectInitializer);
	void StartRespawnDelayTimer();
	FORCEINLINE bool CanRespawn() const { return bCanRespawn; }

private:
	FTimerHandle RespawnDelayTimer;

	UPROPERTY(EditAnywhere, Category="LBlaster|Respawn")
	float RespawnDelay;

	bool bCanRespawn = true;
};
