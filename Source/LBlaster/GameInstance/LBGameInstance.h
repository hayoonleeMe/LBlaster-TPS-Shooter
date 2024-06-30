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
	ULBGameInstance();
	virtual void Init() override;

	// 게임 도중 참가하는 플레이어의 ETeam 값을 캐싱
	UPROPERTY()
	TMap<FUniqueNetIdRepl, ETeam> MidGameJoinedPlayerTeamMap;

	void SetOverallSoundVolume(const UObject* WorldContextObject, float Volume) const;

private:
	inline const static FString SaveGameFileName{ TEXT("MyGame") };

	/*
	 *	Sound Mix
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Sound Mix")
	TObjectPtr<USoundMix> MasterSoundMix;

	/*
	 *	Sound Class
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Sound Class")
	TObjectPtr<USoundClass> MasterSoundClass;
};
