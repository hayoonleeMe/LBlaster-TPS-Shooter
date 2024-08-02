// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChatBox.h"

#include "ChatEntry.h"
#include "Components/Border.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Player/BasePlayerController.h"
#include "Player/LBlasterPlayerState.h"

void UChatBox::InitializeChatBox(EChatMode InChatMode, bool bIsAlwaysExposed)
{
	SetFrameBorderVisibility(bIsAlwaysExposed);
	bAlwaysExposed = bIsAlwaysExposed;
	SetChatMode(InChatMode);
}

void UChatBox::FocusChatEdit()
{
	if (ChatEditText)
	{
		if (ChatModeType != EChatMode::ECM_Lobby && IsValidOwnerController())
		{
			OwnerController->SetInputMode(FInputModeUIOnly());
		}

		ChatEditText->SetFocus();
		SetFrameBorderVisibility(true);
	}
}

void UChatBox::ExitChatEdit()
{
	if (ChatEditText)
	{
		ChatEditText->SetText(FText::GetEmpty());
		SetFrameBorderVisibility(false);
	}
}

void UChatBox::AddChatMessage(const FChatParams& ChatParams)
{
	if (ScrollBox && ChatEntryClass && IsValidOwnerController())
	{
		if (UChatEntry* ChatEntry = CreateWidget<UChatEntry>(OwnerController, ChatEntryClass))
		{
			// 팀 데스매치에서 전달받은 채팅 메시지가 아군 or 적군이 보냈는지 판별
			bool bFriendly = false;
			if (ChatParams.ChatMode == EChatMode::ECM_All || ChatParams.ChatMode == EChatMode::ECM_FriendlyTeam)
			{
				const ALBlasterPlayerState* ReceiverPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>();
				bFriendly = (ReceiverPlayerState != nullptr) && (ChatParams.SenderPlayerTeam == ReceiverPlayerState->GetTeam());
			}
			ChatEntry->SetChatEntryText(ChatParams, bFriendly);

			ScrollBox->AddChild(ChatEntry);
			ScrollBox->ScrollToEnd();
		}
	}
}

void UChatBox::Scroll(float InScrollValue) const
{
	if (ScrollBox && ChatEditText && ChatEditText->GetIsEnabled())
	{
		float NewOffset = ScrollBox->GetScrollOffset() + InScrollValue * ScrollModifier;
		NewOffset = FMath::Clamp(NewOffset, 0, ScrollBox->GetScrollOffsetOfEnd());
		ScrollBox->SetScrollOffset(NewOffset);
	}
}

void UChatBox::ChangeChatMode()
{
	if (ChatModeType != EChatMode::ECM_All && ChatModeType != EChatMode::ECM_FriendlyTeam)
	{
		return;
	}

	// 0 ~ 1
	if (IsValidOwnerController() && ChatEditText && ChatEditText->HasUserFocus(OwnerController))
	{
		const EChatMode NewChatMode = static_cast<EChatMode>((static_cast<uint8>(ChatModeType) + 1) % 2);
		SetChatMode(NewChatMode);
	}
}

void UChatBox::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatEditText)
	{
		ChatEditText->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted);
	}
}

void UChatBox::SetChatMode(EChatMode InChatMode)
{
	ChatModeType = InChatMode;

	if (ChatTargetText)
	{
		const FString ChatTargetPrefix = ChatTextStyle::GetChatTargetPrefix(ChatModeType);
		ChatTargetText->SetText(FText::FromString(ChatTargetPrefix));
	}
}

FReply UChatBox::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (ChatModeType != EChatMode::ECM_Lobby && InKeyEvent.GetKey() == EKeys::Tab)
	{
		ChangeChatMode();
		return FReply::Handled();
	}
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UChatBox::SetFrameBorderVisibility(bool bVisible)
{
	// 항상 보이게
	if (bAlwaysExposed)
	{
		return;
	}

	if (FrameBorder)
	{
		FrameBorder->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (TextFrameBorder)
	{
		TextFrameBorder->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (ScrollBox)
	{
		ScrollBox->SetScrollBarVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UChatBox::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsValidOwnerController() || !IsValidOwnerHUD())
	{
		return;
	}

	if (CommitMethod == ETextCommit::OnEnter && !Text.IsEmpty())
	{
		FChatParams ChatParams;
		ChatParams.Content = Text.ToString();
		ChatParams.ChatMode = ChatModeType;

		if (ALBlasterPlayerState* LBPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>())
		{
			ChatParams.SenderPlayerName = LBPlayerState->GetPlayerName();
			ChatParams.SenderPlayerTeam = LBPlayerState->GetTeam();

			if (ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(OwnerController))
			{
				BasePlayerController->ServerSendChatText(ChatParams);
			}
		}
	}

	// 로비에서 생성됨
	if (ChatModeType == EChatMode::ECM_Lobby)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (ChatEditText)
			{
				ChatEditText->SetText(FText::GetEmpty());
			}
		}
	}
	// 게임 내에서 생성됨
	else
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (ChatEditText)
			{
				ChatEditText->SetText(FText::GetEmpty());
			}
		}
		SetFrameBorderVisibility(false);
		OwnerController->SetInputMode(FInputModeGameOnly());
	}
}
