// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHUD.h"
#include "LBTypes/CrosshairTexture.h"
#include "LBTypes/EquipSlot.h"
#include "LBTypes/Team.h"
#include "LBTypes/WeaponTypes.h"
#include "LBlasterHUD.generated.h"

DECLARE_DELEGATE_OneParam(FOnDesiredCrosshairVisibilityChanged, bool /* bDesiredCrosshairVisibility */);

/**
 * 
 */
UCLASS()
class LBLASTER_API ALBlasterHUD : public ABaseHUD
{
	GENERATED_BODY()

public:
	ALBlasterHUD();
	virtual void DrawHUD() override;
	
	void SetHUDHealth(float InHealth, float InMaxHealth);
	void SetHUDKillScore(int32 InKillScore);
	void SetHUDDeath(int32 InDeath);
	void SetHUDAmmo(int32 InAmmo);
	void SetHUDCarriedAmmo(int32 InCarriedAmmo);
	void SetHUDWeaponTypeText(const FString& InWeaponTypeString);
	void SetHUDMatchCountdown(float InCountdownTime, bool bPlayAnimation);
	void SetHUDGrenadeAmount(int32 InGrenadeAmount);
	void AddCharacterOverlay();
	void RemoveCharacterOverlay();
	void AddMatchTimeTimer();
	void AddAnnouncement();
	void HideAnnouncement();
	void AddElimAnnouncement(const FString& AttackerName, const FString& VictimName);
	void InitSniperScope(const TSubclassOf<UUserWidget>& InSniperScopeClass);
	void ShowSniperScopeWidget(bool bShowScope);
	void HighPingWarning(float InDuration) const;
	bool ShowPauseMenu();
	void FocusChat() const;
	void ScrollChatBox(float InScrollValue) const;
	void SetWeaponSlotIcon(EEquipSlot InEquipSlot, EWeaponType InWeaponType) const;
	void ChooseWeaponSlot(EEquipSlot InEquipSlot) const;
	void ChangeChatMode() const;
	void SetHelpInfoVisibility(bool bVisible);
	
	/*
	 *	BaseHUD
	 */
	virtual void CreateSettingMenu() override;
	virtual void CreateVideoSettingMenu() override;
	virtual void CreateMouseSettingMenu() override;
	virtual void CreateAudioSettingMenu() override;
	virtual void ReturnMenu(bool bForceReturn = false) override;
	virtual void AddNewMenuToStack(ULBlasterUserWidget* InNewMenu) override;
	virtual void AddChatMessage(const FChatParams& ChatParams) override;

	/*
	 *	Scoreboard
	 */
	void UpdateScoreboard();
	void SetScoreboardVisibility(bool bVisible);

	/*
	 *	Mini Scoreboard
	 */
	void SetGoalKillScoreMiniScoreboard(const int32 InGoalKillScore) const;
	void UpdateTeamScoreMiniScoreboard(const ETeam InTeam, const int32 InTeamScore) const;
	void UpdateTotalScoreMiniScoreboard(const int32 InTotalScore) const;

	/*
	 *	Respawn Timer
	 */
	void StartRespawnTimer(float InElimDelay, float InRespawnTimerUpdateFrequency);
	void HideRespawnTimer();

	/*
	 *	Result Menu
	 */
	void AddResultMenu();
	void SetHUDMatchCooldown(float InTime);

	/*
	 *	Crosshair
	 */
	void UpdateCrosshair(float InCrosshairSpread, const FLinearColor& InCrosshairColor);
	void DrawCrosshair(const FCrosshairTexture& CrosshairTexture) const;
	void SetPlayerNameText(const FString& InPlayerName) const;
	void MarkPlayerHit(bool bElimination) const;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> OwnerController;

	bool IsValidOwnerController();
	
	/*
	 *	Crosshair
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	TSubclassOf<class UCrosshair> CrosshairClass;

	UPROPERTY()
	TObjectPtr<UCrosshair> Crosshair;

	void AddCrosshair();
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Crosshair")
	float CrosshairSpreadMax;

	float CrosshairSpread;
	FLinearColor CrosshairColor;
	
	/*
	 *	Sniper Scope
	 */
	UPROPERTY()
	TObjectPtr<class USniperScope> SniperScope;

	/*
	 *	Character Overlay
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Character Overlay")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	TObjectPtr<class UCharacterOverlay> CharacterOverlay;

	/*
	 *	Match Time Timer
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Match Time Timer")
	TSubclassOf<class UMatchTimeTimer> MatchTimeTimerClass;

	UPROPERTY()
	TObjectPtr<UMatchTimeTimer> MatchTimeTimer;

	/*
	 *	Announcement Overlay
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Announcement Overlay")
	TSubclassOf<UUserWidget> AnnouncementClass;

	UPROPERTY()
	TObjectPtr<class UAnnouncement> Announcement;

	/*
	 *	Elim Announcement
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Elim Announcement")
	TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass;

	UPROPERTY(EditAnywhere, Category="LBlaster|Elim Announcement")
	float ElimAnnouncementTime;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(class UElimAnnouncement* MessageToRemove);

	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages;

	/*
	 *	Chat
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Chat")
	TSubclassOf<class UChatUI> ChatUIClass;

	UPROPERTY()
	TObjectPtr<UChatUI> ChatUI;

	void AddChatUI();

	/*
	 *	Pause Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Pause Menu")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	TObjectPtr<class UPauseMenu> PauseMenu;

	bool bShowedPauseMenu = false;

	/*
	 *	Setting Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Setting")
	TSubclassOf<class USettingsHubMenu> SettingMenuClass;

	UPROPERTY()
	TObjectPtr<USettingsHubMenu> SettingMenu;

	/*
	 *	Video Setting Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Setting")
	TSubclassOf<class UVideoSettingMenu> VideoSettingMenuClass;

	UPROPERTY()
	TObjectPtr<UVideoSettingMenu> VideoSettingMenu;

	/*
	 *	Mouse Setting Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Setting")
	TSubclassOf<class UMouseSettingMenu> MouseSettingMenuClass;

	UPROPERTY()
	TObjectPtr<UMouseSettingMenu> MouseSettingMenu;

	/*
	 *	Audio Setting Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Setting")
	TSubclassOf<class UAudioSettingMenu> AudioSettingMenuClass;

	UPROPERTY()
	TObjectPtr<UAudioSettingMenu> AudioSettingMenu;

	/*
	 *	Scoreboard
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Scoreboard")
	TMap<EMatchMode, TSubclassOf<class UScoreboard>> ScoreboardClassByMatchModeMap; 

	UPROPERTY()
	TObjectPtr<UScoreboard> Scoreboard;

	void AddScoreboard();

	/*
	 *	Mini Scoreboard
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Mini Scoreboard")
	TMap<EMatchMode, TSubclassOf<class UMiniScoreboard>> MiniScoreboardClassByMatchModeMap; 

	UPROPERTY()
	TObjectPtr<UMiniScoreboard> MiniScoreboard;

	void AddMiniScoreboard();

	/*
	 *	Respawn Timer
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Respawn Timer")
	TSubclassOf<class URespawnTimer> RespawnTimerClass;

	UPROPERTY()
	TObjectPtr<URespawnTimer> RespawnTimer;

	FTimerHandle RespawnTimerHandle;

	void UpdateHUDRespawnTimer();

	float RespawnTimerUnitPercent = 0.f;

	/*
	 *	HelpInfo
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|HelpInfo")
	TSubclassOf<UUserWidget> HelpInfoClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> HelpInfo;

	void AddHelpInfo();

	FOnDesiredCrosshairVisibilityChanged OnDesiredCrosshairVisibilityChanged;
	

	/*
	 *	Result Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Result Menu")
	TMap<EMatchMode, TSubclassOf<class UResultMenu>> ResultMenuClassByMatchModeMap; 

	UPROPERTY()
	TObjectPtr<UResultMenu> ResultMenu;
};
