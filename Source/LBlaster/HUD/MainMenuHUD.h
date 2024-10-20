// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHUD.h"
#include "MatchModeTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MainMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AMainMenuHUD : public ABaseHUD
{
	GENERATED_BODY()

public:
	/*
	 *	Menus
	 */
	void CreateStartMenu();
	void CreateSessionListMenu();
	void CreateSessionFromMenu(EMatchMode InMatchModeType, int32 NumMaxPlayer, int32 InGoalKillScore);
	void FindSessionsFromMenu();
	void RefreshSessionList();
	void JoinSessionFromMenu(const FOnlineSessionSearchResult& SessionResult);

	/*
	 * BaseHUD
	 */
	virtual void CreateSettingMenu() override;
	virtual void CreateVideoSettingMenu() override;
	virtual void CreateMouseSettingMenu() override;
	virtual void CreateAudioSettingMenu() override;
	virtual void ReturnMenu(bool bForceReturn = false) override;
	virtual void AddNewMenuToStack(ULBlasterUserWidget* InNewMenu) override;
	
protected:
	virtual void BeginPlay() override;

private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<APlayerController> OwnerController;

	bool IsValidOwnerController();
	
	/*
	 *	Multiplayer Sessions
	 */
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	void OnCreateSessionComplete(bool bWasSuccessful);
	void OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);

	/*
	 *	Main Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Main Menu")
	TSubclassOf<class UMainMenu> MainMenuClass;

	UPROPERTY()
	TObjectPtr<UMainMenu> MainMenu;

	void AddMainMenu();

	/*
	 *	Start Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Start Menu")
	TSubclassOf<class UStartMenu> StartMenuClass;

	UPROPERTY()
	TObjectPtr<UStartMenu> StartMenu;

	/*
	 *	Session List Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Session List Menu")
	TSubclassOf<class USessionListMenu> SessionListMenuClass;

	UPROPERTY()
	TObjectPtr<USessionListMenu> SessionListMenu;

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
};
