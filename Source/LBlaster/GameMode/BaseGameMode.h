// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LBTypes/ChatMode.h"
#include "LBTypes/Team.h"
#include "BaseGameMode.generated.h"

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
	void SendChatTextToAll(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam) const;
	void SendChatTextToSameTeam(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam) const;
};
