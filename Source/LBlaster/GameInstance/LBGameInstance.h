// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LBTypes/Team.h"
#include "LBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	// 게임 도중 참가하는 플레이어의 ETeam 값을 캐싱
	UPROPERTY()
	TMap<FUniqueNetIdRepl, ETeam> MidGameJoinedPlayerTeamMap;

private:
	inline const static FString SaveGameFileName{ TEXT("MyGame") };
};
