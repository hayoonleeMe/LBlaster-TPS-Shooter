// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterPlayerController.h"

#include "Character/LBlasterCharacter.h"
#include "GameFramework/GameMode.h"
#include "GameMode/LBlasterGameMode.h"
#include "HUD/LBlasterHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ALBlasterPlayerController::ALBlasterPlayerController()
{
	TimeSyncFrequency = 5.f;
}

void ALBlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
}

float ALBlasterPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}

void ALBlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	// 클라이언트와 동기화를 위한 Server Time을 얻기 위해 가장 빠르게 호출하는 시점.
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}	
}

void ALBlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALBlasterPlayerController, MatchState);
}

void ALBlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;	
	}
	
	const uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDWarmupCountdown(TimeLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void ALBlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	const float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ALBlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	const float CurrentClientTime = GetWorld()->GetTimeSeconds();
	const float RoundTripTime = CurrentClientTime - TimeOfClientRequest;
	const float CurrentServerTime = TimeServerReceivedClientRequest + RoundTripTime * 0.5f;
	ClientServerDelta = CurrentServerTime - CurrentClientTime;
}

void ALBlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ALBlasterPlayerController::ServerCheckMatchState_Implementation()
{
	if (ALBlasterGameMode* GameMode = Cast<ALBlasterGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime);
	}
}

void ALBlasterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (IsValidHUD() && MatchState == MatchState::WaitingToStart)
	{
		LBlasterHUD->AddAnnouncement();
	}	
}

void ALBlasterPlayerController::SetHUDHealth(float InHealth, float InMaxHealth)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDHealth(InHealth, InMaxHealth);
	}
}

void ALBlasterPlayerController::SetHUDScore(float InScore)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDScore(InScore);
	}
}

void ALBlasterPlayerController::SetHUDDeath(int32 InDeath)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDDeath(InDeath);
	}
}

void ALBlasterPlayerController::SetHUDAmmo(int32 InAmmo)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDAmmo(InAmmo);
	}
}

void ALBlasterPlayerController::SetHUDCarriedAmmo(int32 InCarriedAmmo)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDCarriedAmmo(InCarriedAmmo);
	}
}

void ALBlasterPlayerController::SetHUDWeaponTypeText(const FString& InWeaponTypeString)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDWeaponTypeText(InWeaponTypeString);
	}
}

void ALBlasterPlayerController::SetHUDMatchCountdown(float InCountdownTime)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDMatchCountdown(InCountdownTime);
	}
}

void ALBlasterPlayerController::OnMatchStateSet(FName InState)
{
	MatchState = InState;
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ALBlasterPlayerController::UpdateHUDHealth()
{
	if (ALBlasterCharacter* LBlasterCharacter = Cast<ALBlasterCharacter>(GetPawn()))
	{
		LBlasterCharacter->UpdateHUDHealth();
	}
}

void ALBlasterPlayerController::HandleMatchHasStarted()
{
	if (IsValidHUD())
	{
		LBlasterHUD->AddCharacterOverlay();
		LBlasterHUD->HideAnnouncement();
	}
}

void ALBlasterPlayerController::SetHUDWarmupCountdown(float InCountdownTime)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDWarmupCountdown(InCountdownTime);
	}
}

void ALBlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ServerCheckMatchState();
}

void ALBlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ALBlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Player Controller가 초기화될 때 Health HUD 업데이트
	UpdateHUDHealth();	
}

bool ALBlasterPlayerController::IsValidHUD()
{
	if (!LBlasterHUD)
	{
		LBlasterHUD = Cast<ALBlasterHUD>(GetHUD());
	}
	return LBlasterHUD != nullptr;
}