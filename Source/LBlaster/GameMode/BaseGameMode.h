// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LBTypes/ChatParams.h"
#include "BaseGameMode.generated.h"

namespace MatchState
{
	const FName AfterWarmup = FName(TEXT("AfterWarmup"));
	const FName Cooldown = FName(TEXT("Cooldown"));
}

/**
 * 
 */
UCLASS()
class LBLASTER_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	/*
	 *	ChatBox
	 */
	void SendChatText(const FChatParams& ChatParams) const;

	/*
	 *	Match State
	 */
	void SetMatchStateToCooldown();
};
