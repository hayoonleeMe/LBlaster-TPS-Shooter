// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "LBlaster.h"
#include "Character/LBlasterCharacter.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/LBlasterGameMode.h"
#include "HUD/LBlasterHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ALBlasterPlayerController::ALBlasterPlayerController()
{
	/* Time */
	TimeSyncFrequency = 5.f;

	/* High Ping */
	HighPingThreshold = 50.f;
	WarningDuration = 5.f;
	CheckPingFrequency = 15.f;

	/* Input */
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/LBlaster/Core/Inputs/IMC_LBContext.IMC_LBContext'"));
	if (DefaultMappingContextRef.Object)
	{
		DefaultMappingContext = DefaultMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> PauseMenuMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/LBlaster/Core/Inputs/IMC_PauseMenuContext.IMC_PauseMenuContext'"));
	if (PauseMenuMappingContextRef.Object)
	{
		PauseMenuMappingContext = PauseMenuMappingContextRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> PauseMenuActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_PauseMenu.IA_PauseMenu'"));
	if (PauseMenuActionRef.Object)
	{
		PauseMenuAction = PauseMenuActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> FocusChatActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_FocusChat.IA_FocusChat'"));
	if (FocusChatActionRef.Object)
	{
		FocusChatAction = FocusChatActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ChatScrollActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_ChatScroll.IA_ChatScroll'"));
	if (ChatScrollActionRef.Object)
	{
		ChatScrollAction = ChatScrollActionRef.Object;
	}
}

void ALBlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
}

void ALBlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Triggered, this, &ThisClass::ShowPauseMenu);
	EnhancedInputComponent->BindAction(FocusChatAction, ETriggerEvent::Triggered, this, &ThisClass::FocusChat);
	EnhancedInputComponent->BindAction(ChatScrollAction, ETriggerEvent::Triggered, this, &ThisClass::ChatScroll);
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

void ALBlasterPlayerController::UpdateHUDGrenadeAmount()
{
	if (ALBlasterCharacter* LBlasterCharacter = Cast<ALBlasterCharacter>(GetPawn()))
	{
		UpdateHUDGrenadeAmount(LBlasterCharacter->GetGrenadeAmount());
	}
}

void ALBlasterPlayerController::UpdateHUDGrenadeAmount(int32 InGrenadeAmount)
{
	if (IsValidHUD())
	{
		LBlasterHUD->SetHUDGrenadeAmount(InGrenadeAmount);
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

void ALBlasterPlayerController::StartCheckPing()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(CheckPingHandle, this, &ThisClass::CheckPing, CheckPingFrequency, true);

		// 초반에 Ping 체크 한번 수행
		FTimerHandle TempTimer;
		GetWorld()->GetTimerManager().SetTimer(TempTimer, this, &ThisClass::CheckPing, 2.f);
	}
}

void ALBlasterPlayerController::CheckPing()
{
	if (GetPlayerState<APlayerState>() && GetPlayerState<APlayerState>()->GetCompressedPing() * 4 > HighPingThreshold)
	{
		if (IsValidHUD())
		{
			LBlasterHUD->HighPingWarning(WarningDuration);
		}
	}
}

void ALBlasterPlayerController::ClientAddChatText_Implementation(const FText& InText)
{
	if (IsValidHUD())
	{
		LBlasterHUD->AddChatMessage(InText);
	}
}

void ALBlasterPlayerController::ClientElimAnnouncement_Implementation(APlayerState* AttackerState, APlayerState* VictimState)
{
	if (const APlayerState* SelfState = GetPlayerState<APlayerState>(); AttackerState && VictimState && IsValidHUD())
	{
		if (AttackerState == SelfState && VictimState != SelfState)
		{
			LBlasterHUD->AddElimAnnouncement(TEXT("You"), VictimState->GetPlayerName());
			return;
		}
		if (VictimState == SelfState && AttackerState != SelfState)
		{
			LBlasterHUD->AddElimAnnouncement(AttackerState->GetPlayerName(), TEXT("You"));
			return;
		}
		if (AttackerState == VictimState && AttackerState == SelfState)
		{
			LBlasterHUD->AddElimAnnouncement(TEXT("You"), TEXT("Yourself"));
			return;
		}
		if (AttackerState == VictimState && AttackerState != SelfState)
		{
			LBlasterHUD->AddElimAnnouncement(AttackerState->GetPlayerName(), AttackerState->GetPlayerName());
			return;
		}
		LBlasterHUD->AddElimAnnouncement(AttackerState->GetPlayerName(), VictimState->GetPlayerName());
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

void ALBlasterPlayerController::EnablePauseMenuMappingContext() const
{
	if (GEngine && GetWorld() && DefaultMappingContext && PauseMenuMappingContext)
	{
		if (const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(GetWorld()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->RemoveMappingContext(DefaultMappingContext);
				Subsystem->AddMappingContext(PauseMenuMappingContext, 0);
			}	
		}
	}
}

void ALBlasterPlayerController::DisablePauseMenuMappingContext() const
{
	if (GEngine && GetWorld() && DefaultMappingContext && PauseMenuMappingContext)
	{
		if (const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(GetWorld()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->RemoveMappingContext(PauseMenuMappingContext);
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}	
		}
	}
}

void ALBlasterPlayerController::BroadcastElim(APlayerState* AttackerState, APlayerState* VictimState)
{
	ClientElimAnnouncement(AttackerState, VictimState);
}

void ALBlasterPlayerController::BroadcastChatText(const FText& InText)
{
	ClientAddChatText(InText);
}

void ALBlasterPlayerController::ServerSendChatText_Implementation(const FText& InText)
{
	if (IsValidGameMode())
	{
		LBlasterGameMode->SendChatText(InText);
	}
}

void ALBlasterPlayerController::ShowPauseMenu()
{
	if (IsValidHUD())
	{
		if (LBlasterHUD->ShowPauseMenu())
		{
			if (const ALBlasterCharacter* OwningCharacter = Cast<ALBlasterCharacter>(GetCharacter()))
			{
				OwningCharacter->ReleaseCombatState();
			}
		}
	}		
}

void ALBlasterPlayerController::FocusChat()
{
	if (IsValidHUD())
	{
		LBlasterHUD->FocusChat();
	}
}

void ALBlasterPlayerController::ChatScroll(const FInputActionValue& ActionValue)
{
	if (IsValidHUD())
	{
		LBlasterHUD->ScrollChatBox(ActionValue.Get<float>());
	}
}

void ALBlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ServerCheckMatchState();

	// 핑은 클라에서만 체크
	if (!HasAuthority() && IsLocalController())
	{
		StartCheckPing();
	}
}

void ALBlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ALBlasterCharacter* OwningCharacter = Cast<ALBlasterCharacter>(GetCharacter()))
    {
		if (HasAuthority())
		{
    		OwningCharacter->UpdatePlayerNameToOverheadWidget();
			if (IsLocalController())
			{
				UpdateHUDHealth();
				UpdateHUDGrenadeAmount();
				OwningCharacter->EquipDefaultWeapon();
			}	
		}
    }
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
