// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/MainMenuUserWidget.h"
#include "SessionListMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API USessionListMenu : public UMainMenuUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;
	void InitializeSessionListView(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UListView> SessionListView;

	UFUNCTION(BlueprintCallable)
	void OnListViewItemSelectionChanged(bool bIsSelected);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> JoinButton;

	UFUNCTION()
	void OnJoinButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UFUNCTION()
	void OnReturnButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> RefreshButton;

	UFUNCTION()
	void OnRefreshButtonClicked();
};
