// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LBlasterUserWidget.h"
#include "LBTypes/ChatMode.h"
#include "LBTypes/Team.h"
#include "ChatBox.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UChatBox : public ULBlasterUserWidget
{
	GENERATED_BODY()

public:
	void InitializeChatBox(EChatMode InChatMode, bool bIsAlwaysExposed);
	void FocusChatEdit();
	void ExitChatEdit();
	void AddChatMessage(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam);
	void Scroll(float InScrollValue) const;
	void ChangeChatMode();

	FORCEINLINE EChatMode GetChatMode() const { return ChatModeType; }

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
protected:
	virtual void NativeConstruct() override;
	void SetChatMode(EChatMode InChatMode);
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	bool bAlwaysExposed = false;
	
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

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> ChatTargetText;

	/*
	 *	Frame Border
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> FrameBorder;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> TextFrameBorder;

	void SetFrameBorderVisibility(bool bVisible);

	/*
	 *	Chat Mode
	 */
	EChatMode ChatModeType = EChatMode::ECM_All;
};
