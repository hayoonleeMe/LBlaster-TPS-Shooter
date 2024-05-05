// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD/MainMenuUserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "StartMenu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UStartMenu : public UMainMenuUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void MenuSetup() override;
	
	void OnCreateSessionComplete(bool bWasSuccessful);
	void OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);

private:
	/*
	 *	Widgets
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> HostButton;

	UFUNCTION()
	void OnHostButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;

	UFUNCTION()
	void OnJoinButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UFUNCTION()
	void OnReturnButtonClicked();
};
