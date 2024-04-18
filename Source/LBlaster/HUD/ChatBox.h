// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "ChatBox.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UChatBox : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	void FocusChatEdit();
	void ExitChatEdit();
	void AddChatMessage(const FText& InText);
	void Scroll(float InScrollValue) const;

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
protected:
	virtual void NativeConstruct() override;

private:
	/*
	 *	Chat
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> ChatEditText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> ScrollBox;

	UPROPERTY(EditAnywhere, Category="LBlaster|Chat")
	TSubclassOf<class UChatEntry> ChatEntryClass;

	UPROPERTY(EditAnywhere, Category="LBlaster|Chat")
	float ScrollModifier = 60.f;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ChatBoxFadeOut;
	
	UPROPERTY(EditAnywhere, Category="LBlaster|Chat")
	float ChatBoxFadeOutWaitTime = 3.f;

	UPROPERTY(EditAnywhere, Category="LBlaster|Chat")
	float ChatBoxFadeOutDuration = 3.f;
	
	FTimerHandle ChatBoxFadeOutTimer;

	void StartChatBoxFadeOutTimer();
};
