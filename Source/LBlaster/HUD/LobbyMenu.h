// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyMenu.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API ULobbyMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;
	
	void OnDestroySessionComplete(bool bWasSuccessful);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> StartButton;

	UFUNCTION()
	void OnStartButtonClicked();
	
	void Travel();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> QuitButton;

	UFUNCTION()
	void OnQuitButtonClicked();

	void DestroyAllClientSession();

	// 서버에서 QuitButton을 눌렀을 때 true로 설정됨. Tick에서 ServerTravel을 할 수 있는지 체크하는 데 사용됨.
	bool bWantServerTravel = false;
};
