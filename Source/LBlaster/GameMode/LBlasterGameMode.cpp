// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterGameMode.h"

#include "Character/LBlasterCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "HUD/LBlasterHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LBlasterPlayerController.h"
#include "Player/LBlasterPlayerState.h"

ALBlasterGameMode::ALBlasterGameMode()
{
	/* Default Class */
	static ConstructorHelpers::FClassFinder<ALBlasterCharacter> LBlasterCharacterClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Actors/Manny/BP_LBlasterCharacter.BP_LBlasterCharacter_C'"));
	if (LBlasterCharacterClassRef.Class)
	{
		DefaultPawnClass = LBlasterCharacterClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<ALBlasterHUD> LBlasterHUDClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/HUD/BP_LBlasterHUD.BP_LBlasterHUD_C'"));
	if (LBlasterHUDClassRef.Class)
	{
		HUDClass = LBlasterHUDClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<ALBlasterPlayerController> PlayerControllerClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/Controllers/Player/BP_LBlasterPlayerController.BP_LBlasterPlayerController_C'"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<ALBlasterPlayerState> PlayerStateClassRef(TEXT("/Script/Engine.Blueprint'/Game/LBlaster/Core/States/Player/BP_LBlasterPlayerState.BP_LBlasterPlayerState_C'"));
	if (PlayerStateClassRef.Class)
	{
		PlayerStateClass = PlayerStateClassRef.Class;
	}

	bDelayedStart = true;
	WarmupTime = 10.f;
	MatchTime = 120.f;
	CooldownTime = 10.f;
}

void ALBlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = WarmupTime + MatchTime + CooldownTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ALBlasterGameMode::PlayerEliminated(ALBlasterCharacter* EliminatedCharacter, ALBlasterPlayerController* VictimController,
                                         ALBlasterPlayerController* AttackerController)
{
}

void ALBlasterGameMode::RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController)
{
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Reset();
		EliminatedCharacter->Destroy();
	}
	if (EliminatedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		const int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(EliminatedController, PlayerStarts[Selection]);
	}
}

void ALBlasterGameMode::PlayerLeftGame(ALBlasterCharacter* LeftCharacter)
{
	if (LeftCharacter)
	{
		LeftCharacter->Elim(true);
	}
}

void ALBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ALBlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	// 모든 Player Controller에 접근하는 Iterator
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(*It))
		{
			PlayerController->OnMatchStateSet(MatchState);
		}
	}
}
