// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "LobbyMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULobbyMenu : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	virtual void MenuSetup() override;

	void OnDestroySessionComplete(bool bWasSuccessful);
	void SetNumMaxPlayersText(int32 InNumMaxPlayers);
	void ReturnMenuByKeyboard();
	
protected:
	void SetNumPlayersText();
	void SetNumPlayersText(int32 InNumCurrentPlayers, int32 InNumMaxPlayers);
	void AddNumCurrentPlayersText(int32 AmountToAdd);

	UPROPERTY(EditAnywhere, Category="LBlaster|Player List")
	TSubclassOf<class UPlayerListRow> PlayerListRowClass;

	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerState> OwnerPlayerState;

	bool IsValidOwnerPlayerState();
	
private:
	/*
	 *	Players Text
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> NumPlayersText;

	int32 NumCurrentPlayers = 0;
	int32 NumMaxPlayers = 0;

	/*
	 *	Goal Kill Score
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> GoalKillScoreText;

	void UpdateGoalKillScoreText();

	/*
	 *	Server Menu Button
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UHorizontalBox> ServerBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> StartButton;

	UFUNCTION()
	void OnStartButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ServerReturnButton;

	UFUNCTION()
	void OnReturnButtonClicked();

	/*
	 *	Client Menu Button
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> ClientBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ClientReturnButton;

	/*
	 *	Return Alert Overlay
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UOverlay> ReturnAlertOverlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ReturnAlertTextBlock;

	inline const static FString ServerReturnAlertText{ TEXT("세션을 제거하시겠습니까?") };
	inline const static FString ClientReturnAlertText{ TEXT("세션에서 나가시겠습니까?") };

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ReturnAlertAcceptButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ReturnAlertCancelButton;
	
	UFUNCTION()
	void OnReturnAlertCancelButtonClicked();
};
