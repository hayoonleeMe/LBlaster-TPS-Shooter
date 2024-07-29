// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterGameMode.h"

#include "MultiplayerSessionsSubsystem.h"
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

	WarmupTime = 10.f;
#if WITH_EDITOR
	WarmupTime = 1.f;
#endif
	MatchTime = 900.f;
	CooldownTime = 10.f;
}

void ALBlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::AfterWarmup);
		}
	}
	else if (MatchState == MatchState::AfterWarmup)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = WarmupTime + RemainMatchTime + CooldownTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f && !bAlreadyReturnToMainMenu)
		{
			bAlreadyReturnToMainMenu = true;
			ReturnToMainMenuWithAllPlayers();
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
		RestartPlayer(EliminatedController);
	}
}

void ALBlasterGameMode::PlayerLeftGame(ALBlasterCharacter* LeftCharacter)
{
	if (LeftCharacter)
	{
		LeftCharacter->Elim(true);
	}
}

void ALBlasterGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(NewPlayer))
	{
		PlayerController->bCanSetInvincibilityInBeginPlay = true;
	}
}

void ALBlasterGameMode::OnDestroySessionComplete(bool bWasSuccessful)
{
	// 실패하면 다시 수행.
	if (!bWasSuccessful)
	{
		ReturnToMainMenuWithAllPlayers();
		return;
	}
	
	ReturnToMainMenuHost();
}

void ALBlasterGameMode::DestroyAllClientSession() const
{
	// 호스트를 제외한 모든 클라이언트의 플레이어 컨트롤러에서 Client RPC를 호출해 DestroySession을 호출하게 한다.
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PlayerController = It->Get(); PlayerController != GetWorld()->GetFirstPlayerController())
			{
				if (ABasePlayerController* SHController = Cast<ABasePlayerController>(PlayerController))
				{
					SHController->ClientDestroySession();
				}
			}
		}	
	}
}

void ALBlasterGameMode::ReturnToMainMenuWithAllPlayers()
{
	if (MultiplayerSessionsSubsystem)
	{
		DestroyAllClientSession();
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void ALBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();

	/* MultiplayerSessions */
	if (UGameInstance* GameInstance = GetGameInstance())
    {
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->LBOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);
	}
}

void ALBlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	// Early Game End
	if (MatchState == MatchState::Cooldown)
	{
		RemainMatchTime = FMath::Max(0.f, MatchTime - CountdownTime);
	}
	
	// 모든 Player Controller에 접근하는 Iterator
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(*It))
		{
			if (MatchState == MatchState::Cooldown)
			{
				PlayerController->OnMatchStateSet(MatchState, RemainMatchTime);
			}
			else
			{
				PlayerController->OnMatchStateSet(MatchState);
			}
		}
	}
}
