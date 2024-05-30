// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterGameMode.h"
#include "TeamDeathMatchGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ATeamDeathMatchGameMode : public ALBlasterGameMode
{
	GENERATED_BODY()

public:
	ATeamDeathMatchGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void PlayerEliminated(ALBlasterCharacter* EliminatedCharacter, ALBlasterPlayerController* VictimController, ALBlasterPlayerController* AttackerController) override;
	
	UFUNCTION(BlueprintCallable)
	AActor* FindPlayerStartTDM(AController* Player);

private:
	UPROPERTY(EditAnywhere, Category="LBlaster")
	TSubclassOf<class ARedTeamPlayerStart> RedTeamPlayerStartClass;

	UPROPERTY(EditAnywhere, Category="LBlaster")
	TSubclassOf<class ABlueTeamPlayerStart> BlueTeamPlayerStartClass;
};
