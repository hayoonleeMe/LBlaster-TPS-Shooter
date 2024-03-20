// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatBox.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UChatBox : public UUserWidget
{
	GENERATED_BODY()

public:
	void FocusChatEdit() const;
	void ExitChatEdit() const;
	void AddChatMessage(const FText& InText);
	void Scroll(float InScrollValue) const;

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
protected:
	virtual void NativeConstruct() override;

private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> PlayerController;

	bool IsValidPlayerController();

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
};
