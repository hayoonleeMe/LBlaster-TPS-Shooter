// Fill out your copyright notice in the Description page of Project Settings.


#include "LBlasterPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "LBlaster.h"
#include "Character/LBlasterCharacter.h"
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
	HighPingThreshold = 100.f;
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

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MenuMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/LBlaster/Core/Inputs/IMC_MenuContext.IMC_MenuContext'"));
	if (MenuMappingContextRef.Object)
	{
		MenuMappingContext = MenuMappingContextRef.Object;
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
	
	static ConstructorHelpers::FObjectFinder<UInputAction> ChangeChatModeActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_ChangeChatMode.IA_ChangeChatMode'"));
	if (ChangeChatModeActionRef.Object)
	{
		ChangeChatModeAction = ChangeChatModeActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ReturnMenuActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_ReturnMenu.IA_ReturnMenu'"));
	if (ReturnMenuActionRef.Object)
	{
		ReturnMenuAction = ReturnMenuActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ScoreboardActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_Scoreboard.IA_Scoreboard'"));
	if (ScoreboardActionRef.Object)
	{
		ScoreboardAction = ScoreboardActionRef.Object;
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
	
	/* IMC_LBContext */
	EnhancedInputComponent->BindAction(FocusChatAction, ETriggerEvent::Triggered, this, &ThisClass::FocusChat);
	EnhancedInputComponent->BindAction(ChatScrollAction, ETriggerEvent::Triggered, this, &ThisClass::ChatScroll);
	EnhancedInputComponent->BindAction(ChangeChatModeAction, ETriggerEvent::Triggered, this, &ThisClass::ChangeChatMode);
	EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Triggered, this, &ThisClass::ShowScoreboard);

	/* IMC_PauseMenuContext */
	EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Triggered, this, &ThisClass::ShowPauseMenu);

	/* IMC_MenuContext */
	EnhancedInputComponent->BindAction(ReturnMenuAction, ETriggerEvent::Triggered, this, &ThisClass::ReturnMenu);
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

void ALBlasterPlayerController::Destroyed()
{
	// 클라이언트의 비정상적인 게임 종료(ex.강제종료)로 인해 해당 플레이어의 오브젝트 들이 제대로 제거되지 않았을 때
	if (HasAuthority() && !bAlreadyServerLeaveGame)
	{
		ServerLeaveGame();
	}
	// 호스트의 비정상적인 게임 종료(ex. 강제종료)로 인해 클라이언트 플레이어들이 세션에서 해제되지 않았을 때
	if (!HasAuthority() && !bAlreadyServerLeaveGame)
	{
		ClientDestroySession();
	}
	
	Super::Destroyed();
}

void ALBlasterPlayerController::SetHUDTime()
{
	if (HasAuthority() && IsValidOwnerGameMode())
	{
		LevelStartingTime = OwnerGameMode->LevelStartingTime;
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
	if (HasAuthority() && IsValidOwnerGameMode())
	{
		SecondsLeft = FMath::CeilToInt(OwnerGameMode->GetCountdownTime());
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
	SingleTripTime = RoundTripTime * 0.5f;
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
	if (IsValidOwnerGameMode())
	{
		WarmupTime = OwnerGameMode->WarmupTime;
		MatchTime = OwnerGameMode->MatchTime;
		CooldownTime = OwnerGameMode->CooldownTime;
		LevelStartingTime = OwnerGameMode->LevelStartingTime;
		MatchState = OwnerGameMode->GetMatchState();
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

	if (IsValidOwningHUD() && MatchState == MatchState::WaitingToStart)
	{
		OwningHUD->AddAnnouncement();
	}	
}

void ALBlasterPlayerController::SetHUDHealth(float InHealth, float InMaxHealth)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDHealth(InHealth, InMaxHealth);
	}
}

void ALBlasterPlayerController::SetHUDKillScore(int32 InKillScore)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDKillScore(InKillScore);
	}
}

void ALBlasterPlayerController::SetHUDDeath(int32 InDeath)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDDeath(InDeath);
	}
}

void ALBlasterPlayerController::SetHUDAmmo(int32 InAmmo)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDAmmo(InAmmo);
	}
}

void ALBlasterPlayerController::SetHUDCarriedAmmo(int32 InCarriedAmmo)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDCarriedAmmo(InCarriedAmmo);
	}
}

void ALBlasterPlayerController::SetHUDWeaponTypeText(const FString& InWeaponTypeString)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDWeaponTypeText(InWeaponTypeString);
	}
}

void ALBlasterPlayerController::SetHUDMatchCountdown(float InCountdownTime)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDMatchCountdown(InCountdownTime);
	}
}

void ALBlasterPlayerController::SetHUDAnnouncementCountdown(float InCountdownTime)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDAnnouncementCountdown(InCountdownTime);
	}
}

void ALBlasterPlayerController::UpdateHUDGrenadeAmount()
{
	if (IsValidOwningCharacter())
	{
		UpdateHUDGrenadeAmount(OwningCharacter->GetGrenadeAmount());
	}
}

void ALBlasterPlayerController::UpdateHUDGrenadeAmount(int32 InGrenadeAmount)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDGrenadeAmount(InGrenadeAmount);
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
	if (IsValidOwningCharacter() && GetPlayerState<APlayerState>())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(0, 10.f, FColor::Orange, FString::Printf(TEXT("Ping : %d"), GetPlayerState<APlayerState>()->GetCompressedPing() * 4));
		}
		
		// SSR 제한
		if (GetPlayerState<APlayerState>()->GetCompressedPing() * 4 > HighPingThreshold)
		{
			if (IsValidOwningHUD())
			{
				OwningCharacter->EnableServerSideRewind(false);
				OwningHUD->HighPingWarning(WarningDuration);
			}	
		}
		// SSR 사용 가능
		else
		{
			OwningCharacter->EnableServerSideRewind(true);
		}
	}
}

void ALBlasterPlayerController::ClientElimAnnouncement_Implementation(APlayerState* AttackerState, APlayerState* VictimState)
{
	if (const APlayerState* SelfState = GetPlayerState<APlayerState>(); AttackerState && VictimState && IsValidOwningHUD())
	{
		if (AttackerState == SelfState && VictimState != SelfState)
		{
			OwningHUD->AddElimAnnouncement(TEXT("You"), VictimState->GetPlayerName());
			return;
		}
		if (VictimState == SelfState && AttackerState != SelfState)
		{
			OwningHUD->AddElimAnnouncement(AttackerState->GetPlayerName(), TEXT("You"));
			return;
		}
		if (AttackerState == VictimState && AttackerState == SelfState)
		{
			OwningHUD->AddElimAnnouncement(TEXT("You"), TEXT("Yourself"));
			return;
		}
		if (AttackerState == VictimState && AttackerState != SelfState)
		{
			OwningHUD->AddElimAnnouncement(AttackerState->GetPlayerName(), AttackerState->GetPlayerName());
			return;
		}
		OwningHUD->AddElimAnnouncement(AttackerState->GetPlayerName(), VictimState->GetPlayerName());
	}
}

void ALBlasterPlayerController::UpdateHUDHealth()
{
	if (IsValidOwningCharacter())
	{
		OwningCharacter->UpdateHUDHealth();
	}
}

void ALBlasterPlayerController::HandleMatchHasStarted()
{
	if (IsValidOwningHUD())
	{
		OwningHUD->AddCharacterOverlay();
		OwningHUD->HideAnnouncement();
	}
}

void ALBlasterPlayerController::HandleCooldown()
{
	if (IsValidOwningHUD())
	{
		OwningHUD->RemoveCharacterOverlay();
		OwningHUD->SetCooldownAnnouncement();
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

void ALBlasterPlayerController::EnableMenuMappingContext() const
{
	if (GEngine && GetWorld() && PauseMenuMappingContext && MenuMappingContext)
	{
		if (const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(GetWorld()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->RemoveMappingContext(PauseMenuMappingContext);
				Subsystem->AddMappingContext(MenuMappingContext, 0);
			}	
		}
	}
}

void ALBlasterPlayerController::DisableMenuMappingContext() const
{
	if (GEngine && GetWorld() && PauseMenuMappingContext && MenuMappingContext)
	{
		if (const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(GetWorld()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->RemoveMappingContext(MenuMappingContext);
				Subsystem->AddMappingContext(PauseMenuMappingContext, 0);
			}	
		}
	}
}

void ALBlasterPlayerController::BroadcastElim(APlayerState* AttackerState, APlayerState* VictimState)
{
	ClientElimAnnouncement(AttackerState, VictimState);
}

void ALBlasterPlayerController::SetWeaponSlotIcon(EEquipSlot InEquipSlot, EWeaponType InWeaponType)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetWeaponSlotIcon(InEquipSlot, InWeaponType);
	}
}

void ALBlasterPlayerController::ChooseWeaponSlot(EEquipSlot InEquipSlot)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->ChooseWeaponSlot(InEquipSlot);
	}
}

void ALBlasterPlayerController::ServerLeaveGame_Implementation()
{
	if (GetWorld() && GetCharacter())
	{
		if (ALBlasterGameMode* GameMode = GetWorld()->GetAuthGameMode<ALBlasterGameMode>())
		{
			if (ALBlasterCharacter* LBCharacter = Cast<ALBlasterCharacter>(GetCharacter()))
			{
				if (!bAlreadyServerLeaveGame)
				{
					bAlreadyServerLeaveGame = true;
				}
				
				GameMode->PlayerLeftGame(LBCharacter);
			}
		}	
	}
}



void ALBlasterPlayerController::ShowPauseMenu()
{
	if (IsValidOwningCharacter() && IsValidOwningHUD())
	{
		if (OwningHUD->ShowPauseMenu())
		{
			OwningCharacter->ReleaseCombatState();
		}
	}		
}

void ALBlasterPlayerController::FocusChat()
{
	if (IsValidOwningHUD())
	{
		OwningHUD->FocusChat();
	}
}

void ALBlasterPlayerController::ChatScroll(const FInputActionValue& ActionValue)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->ScrollChatBox(ActionValue.Get<float>());
	}
}

void ALBlasterPlayerController::ChangeChatMode()
{
	if (IsValidOwningHUD())
	{
		OwningHUD->ChangeChatMode();
	}
}

void ALBlasterPlayerController::ReturnMenu()
{
	if (IsValidOwningHUD())
	{
		OwningHUD->ReturnMenu();
	}
}

void ALBlasterPlayerController::ShowScoreboard(const FInputActionValue& ActionValue)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetScoreboardVisibility(ActionValue.Get<bool>());
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

	/* Input Mode */
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

void ALBlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ALBlasterCharacter* PossessCharacter = Cast<ALBlasterCharacter>(InPawn))
	{
		if (HasAuthority())
		{
			PossessCharacter->UpdatePlayerNameToOverheadWidget();
			PossessCharacter->UpdateHUDHealth();
			UpdateHUDGrenadeAmount(PossessCharacter->GetGrenadeAmount());
			PossessCharacter->EquipDefaultWeapon();
		}	
	}
}

bool ALBlasterPlayerController::IsValidOwningCharacter()
{
	if (!OwningCharacter && GetCharacter())
	{
		OwningCharacter = Cast<ALBlasterCharacter>(GetCharacter());
	}
	return OwningCharacter != nullptr;
}

bool ALBlasterPlayerController::IsValidOwningHUD()
{
	if (!OwningHUD && GetHUD())
	{
		OwningHUD = Cast<ALBlasterHUD>(GetHUD());
	}
	return OwningHUD != nullptr;
}

bool ALBlasterPlayerController::IsValidOwnerGameMode()
{
	if (!OwnerGameMode && UGameplayStatics::GetGameMode(this))
	{
		OwnerGameMode = Cast<ALBlasterGameMode>(UGameplayStatics::GetGameMode(this));
	}
	return OwnerGameMode != nullptr;
}
