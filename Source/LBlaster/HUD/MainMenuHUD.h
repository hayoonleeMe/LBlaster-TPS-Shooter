// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MainMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()

public:
	/*
	 *	Menus
	 */
	void CreateStartMenu();
	void CreateSettingMenu();
	void CreateGraphicSettingMenu();
	void CreateSessionListMenu();
	void ReturnMenu();

	/*
	 *	Start Menu
	 */
	void OnCreateSessionButtonClicked(const FString& MatchModeString, int32 NumMaxPlayer);
	void OnFindSessionsButtonClicked();
	void RefreshSessionList();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:
	/*
	 *	Multiplayer Sessions
	 */
	// The subsystem designed to handle all online session functionality
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	void OnCreateSessionComplete(bool bWasSuccessful);
	void OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);

	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<APlayerController> OwnerController;

	bool IsValidOwnerController();

	/*
	 *	Main Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Main Menu")
	TSubclassOf<UUserWidget> MainMenuClass;

	UPROPERTY()
	TObjectPtr<class UMainMenu> MainMenu;

	void AddMainMenu();

	TArray<class UMainMenuUserWidget*> MenuStack;

	void AddNewMenuToStack(UMainMenuUserWidget* InNewMenu);

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
	UPROPERTY(EditAnywhere, Category="LBlaster|Setting Menu")
	TSubclassOf<class USettingMenuForMainMenu> SettingMenuClass;

	UPROPERTY()
	TObjectPtr<USettingMenuForMainMenu> SettingMenu;

	/*
	 *	Graphic Setting Menu
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Graphic Setting Menu")
	TSubclassOf<class UGraphicSettingMenuForMainMenu> GraphicSettingMenuClass;

	UPROPERTY()
	TObjectPtr<UGraphicSettingMenuForMainMenu> GraphicSettingMenu;
};
