// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameMode.h"
#include "LBTypes/ChatMode.h"
#include "LBTypes/Team.h"
#include "LBlasterGameMode.generated.h"

namespace MatchState
{
	const FName Cooldown = FName(TEXT("Cooldown"));
}

/**
 * 
 */
UCLASS()
class LBLASTER_API ALBlasterGameMode : public ABaseGameMode
{
	GENERATED_BODY()

public:
	ALBlasterGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(class ALBlasterCharacter* EliminatedCharacter, class ALBlasterPlayerController* VictimController, ALBlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);
	void PlayerLeftGame(class ALBlasterCharacter* LeftCharacter);

	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }

	UPROPERTY(EditDefaultsOnly, Category="LBlaster|Time")
	float WarmupTime;

	UPROPERTY(EditDefaultsOnly, Category="LBlaster|Time")
	float MatchTime;

	UPROPERTY(EditDefaultsOnly, Category="LBlaster|Time")
	float CooldownTime;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;
};
