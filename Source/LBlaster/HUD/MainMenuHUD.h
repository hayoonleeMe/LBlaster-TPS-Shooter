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
	 *	Multiplayer Sessions
	 */
	UFUNCTION(BlueprintCallable)
	void InitializeMultiplayerSessions(int32 InNumPublicConnections = 4, const FString& InMatchType = FString(TEXT("FreeForAll")), const FString& InLobbyPath = FString(TEXT("/Game/LBlaster/Maps/Lobby")));
	// TODO : 매직 넘버를 상수로 대체 및 블루프린트에서의 호출을 C++에서 호출로 대체

	/*
	 *	Menus
	 */
	void CreateStartMenu();
	void CreateSettingMenu();
	void CreateGraphicSettingMenu();
	void ReturnMenu();

	/*
	 *	Start Menu
	 */
	void OnHostButtonClicked();
	void OnJoinButtonClicked();

protected:
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
