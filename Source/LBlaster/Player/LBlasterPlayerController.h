// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "BasePlayerController.h"
#include "LBTypes/EquipSlot.h"
#include "LBTypes/WeaponTypes.h"
#include "LBlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ALBlasterPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

public:
	ALBlasterPlayerController();
	virtual void Tick(float DeltaSeconds) override;
	// Synced with Server world clock
	virtual float GetServerTime();
	// Sync with server clock as soon as possible (Called after this PlayerController's Viewport/net connection is associated with this player controller)
	virtual void ReceivedPlayer() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Destroyed() override;

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDKillScore(int32 InKillScore);
	void SetHUDDeath(int32 InDeath);
	void SetHUDAmmo(int32 InAmmo);
	void SetHUDCarriedAmmo(int32 InCarriedAmmo);
	void SetHUDWeaponTypeText(const FString& InWeaponTypeString = FString());
	void SetHUDMatchCountdown(float InCountdownTime, bool bPlayAnimation);
	void SetHUDMatchCooldown(float InTime);
	void OnMatchStateSet(FName InState);
	void UpdateHUDHealth();
	void UpdateHUDGrenadeAmount();
	void UpdateHUDGrenadeAmount(int32 InGrenadeAmount);
	void StartRespawnTimer(float InElimDelay, float InRespawnTimerUpdateFrequency);
	void HideRespawnTimer();
	
	UFUNCTION(Client, Reliable)
	void ClientHideRespawnTimer();

	FORCEINLINE float GetSingleTripTime() const { return SingleTripTime; }
	
	void HandleMatchHasStarted();
	void HandleAfterWarmup();
	void HandleCooldown();

	void EnablePauseMenuMappingContext() const;
	void DisablePauseMenuMappingContext() const;
	void EnableMenuMappingContext() const;
	void DisableMenuMappingContext() const;

	void BroadcastElim(APlayerState* AttackerState, APlayerState* VictimState);

	void SetWeaponSlotIcon(EEquipSlot InEquipSlot, EWeaponType InWeaponType);
	void ChooseWeaponSlot(EEquipSlot InEquipSlot);

	/*
	 *	Leave Game
	 */
	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	/*
	 *	Invincibility
	 */
	bool bCanSetInvincibilityInBeginPlay = false;
	void StartInvincibilityTimer();

protected:
	/*
	 *	Input
	 */
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputMappingContext> PauseMenuMappingContext;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputMappingContext> MenuMappingContext;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<class UInputAction> PauseMenuAction;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> FocusChatAction;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> ChatScrollAction;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> ChangeChatModeAction;

	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> ReturnMenuAction;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> ScoreboardAction;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Input")
	TObjectPtr<UInputAction> HelpInfoAction;

	void ShowPauseMenu();
	void FocusChat();
	void ChatScroll(const FInputActionValue& ActionValue);
	void ChangeChatMode();
	void ReturnMenu();
	void ShowScoreboard(const FInputActionValue& ActionValue);
	void ShowHelpInfo(const FInputActionValue& ActionValue);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	void SetHUDTime();

	/*
	 *	Sync time between server and client
	 */
	// current server time을 요청. 요청이 전달될 때의 client time을 전달.
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// ServerRequestServerTime에 대한 응답으로 클라이언트로 current Server Time 전달.
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f;
	float SingleTripTime = 0.f;

	UPROPERTY(EditAnywhere, Category="LBlaster|Time")
	float TimeSyncFrequency;

	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);
	
private:
	/*
	 *	Owning
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwningCharacter;

	bool IsValidOwningCharacter();
	
	UPROPERTY()
	TObjectPtr<class ALBlasterHUD> OwningHUD;

	bool IsValidOwningHUD();

	UPROPERTY()
	TObjectPtr<class ALBlasterGameMode> OwnerGameMode;

	bool IsValidOwnerGameMode();

	bool bAlreadyServerLeaveGame = false;

	/*
     *	Match Countdown
     */
	float LevelStartingTime = 0.f;
    float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
    uint8 CountdownInt = 0.f;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	/*
	 *	High Ping
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|High Ping")
	float HighPingThreshold;

	UPROPERTY(EditAnywhere, Category="LBlaster|High Ping")
	float WarningDuration;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|High Ping")
	float CheckPingFrequency;

	FTimerHandle CheckPingHandle;
	void StartCheckPing();
	void CheckPing();

	/*
	 *	Elim Announcement
	 */
	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* AttackerState, APlayerState* VictimState);

	/*
	 *	Invincibility
	 */
	FTimerHandle InvincibilityTimer;
};
