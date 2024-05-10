// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SessionHelperPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ASessionHelperPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/*
	 *	MultiplayerSessions
	 */
	UFUNCTION(Client, Reliable)
	void ClientDestroySession();
};
