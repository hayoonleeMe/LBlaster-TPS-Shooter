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
	if (HasAuthority() && IsValidGameMode())
	{
		LevelStartingTime = LBlasterGameMode->LevelStartingTime;
	}
	
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;	
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = WarmupTime + MatchTime + CooldownTime - GetServerTime() + LevelStartingTime;
	}
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (HasAuthority() && IsValidGameMode())
	{
		SecondsLeft = FMath::CeilToInt(LBlasterGameMode->GetCountdownTime());
	}
	
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
		else if (MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
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
	if (IsValidGameMode())
	{
		WarmupTime = LBlasterGameMode->WarmupTime;
		MatchTime = LBlasterGameMode->MatchTime;
		CooldownTime = LBlasterGameMode->CooldownTime;
		LevelStartingTime = LBlasterGameMode->LevelStartingTime;
		MatchState = LBlasterGameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void ALBlasterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
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

void ALBlasterPlayerController::SetHUDAnnouncementCountdown(float InCountdownTime)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDAnnouncementCountdown(InCountdownTime);
	}
}

void ALBlasterPlayerController::OnMatchStateSet(FName InState)
{
	MatchState = InState;
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ALBlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
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

void ALBlasterPlayerController::HandleCooldown()
{
	if (IsValidHUD())
	{
		LBlasterHUD->RemoveCharacterOverlay();
		LBlasterHUD->SetCooldownAnnouncement();
	}
}

void ALBlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ServerCheckMatchState();
}

void ALBlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Player Controller가 초기화될 때 Health HUD 업데이트
	UpdateHUDHealth();	
}

bool ALBlasterPlayerController::IsValidHUD()
{
	if (!LBlasterHUD && GetHUD())
	{
		LBlasterHUD = Cast<ALBlasterHUD>(GetHUD());
	}
	return LBlasterHUD != nullptr;
}

bool ALBlasterPlayerController::IsValidGameMode()
{
	if (!LBlasterGameMode && UGameplayStatics::GetGameMode(this))
	{
		LBlasterGameMode = Cast<ALBlasterGameMode>(UGameplayStatics::GetGameMode(this));
	}
	return LBlasterGameMode != nullptr;
}
