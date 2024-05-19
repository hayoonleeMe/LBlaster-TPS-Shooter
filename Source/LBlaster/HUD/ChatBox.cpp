// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChatBox.h"

#include "ChatEntry.h"
#include "LBlasterHUD.h"
#include "LobbyHUD.h"
#include "Components/Border.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Player/LBlasterPlayerController.h"
#include "Player/LBlasterPlayerState.h"

void UChatBox::InitializeChatBox(EChatMode InChatMode, bool bIsAlwaysExposed)
{
	SetIsFocusable(true);
	AddToViewport();
	SetFrameBorderVisibility(bIsAlwaysExposed);
	bAlwaysExposed = bIsAlwaysExposed;
	SetChatMode(InChatMode);
}

void UChatBox::FocusChatEdit()
{
	if (ChatEditText)
	{
		if (IsValidOwnerController())
		{
			OwnerController->SetInputMode(FInputModeUIOnly());
		}
		
		ChatEditText->SetIsEnabled(true);
		ChatEditText->SetFocus();
		SetFrameBorderVisibility(true);
	}
}

void UChatBox::ExitChatEdit()
{
	if (ChatEditText)
	{
		ChatEditText->SetText(FText::GetEmpty());
		ChatEditText->SetIsEnabled(false);
		SetFrameBorderVisibility(false);
	}
}

void UChatBox::AddChatMessage(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam)
{
	if (ScrollBox && ChatEntryClass && IsValidOwnerController())
	{
		if (UChatEntry* ChatEntry = CreateWidget<UChatEntry>(OwnerController, ChatEntryClass))
		{
			const FText FormattedPlayerName = FText::FromString(FString::Printf(TEXT("%s : "), *InPlayerName));
			if (InChatMode == EChatMode::ECM_All)
			{
				if (ALBlasterPlayerState* LBPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>())
				{
					const bool bFriendlyTeam = LBPlayerState->GetTeam() == SourceTeam;
					const FLinearColor& ColorToUse = bFriendlyTeam ? AllForFriendlyTeamColor : AllForOpponentTeamColor;
					ChatEntry->SetChatEntryPrefix(FText::FromString(TEXT("[전체] ")), ColorToUse);
					ChatEntry->SetChatEntryPlayerName(FormattedPlayerName, ColorToUse);
				}
			}
			else if (InChatMode == EChatMode::ECM_FriendlyTeam)
			{
				ChatEntry->EmptyChatEntryPrefix();
				ChatEntry->SetChatEntryPlayerName(FormattedPlayerName, AllForFriendlyTeamColor);
			}
			else if (InChatMode == EChatMode::ECM_OnlyAll)
			{
				// TODO : 개인전 구현 후 다시 테스트
				ChatEntry->EmptyChatEntryPrefix();
				ChatEntry->SetChatEntryPlayerName(FormattedPlayerName, AllForNormalColor);
			}
			ChatEntry->SetChatEntryText(InText);
			
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
	// 0 ~ 1
	if (IsValidOwnerController() && ChatEditText && ChatEditText->HasUserFocus(OwnerController))
	{
		const EChatMode NewChatMode = static_cast<EChatMode>((static_cast<uint8>(ChatModeType) + 1) % (static_cast<uint8>(EChatMode::ECM_Max) - 1));
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
		switch (ChatModeType)
		{
		case EChatMode::ECM_All:
			ChatTargetText->SetText(FText::FromString(TEXT("[전체]")));
			break;

		case EChatMode::ECM_FriendlyTeam:
			ChatTargetText->SetText(FText::FromString(TEXT("[팀]")));
			break;

		case EChatMode::ECM_OnlyAll:
			ChatTargetText->SetText(FText());
			break;
		}	
	}
}

FReply UChatBox::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
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
		if (APlayerState* PlayerState = OwnerController->GetPlayerState<APlayerState>())
		{
			if (ALBlasterPlayerController* LBOwnerController = Cast<ALBlasterPlayerController>(OwnerController))
			{
				if (ChatModeType == EChatMode::ECM_All)
				{
					LBOwnerController->ServerSendChatTextToAll(PlayerState->GetPlayerName(), Text, ChatModeType);
				}
				else if (ChatModeType == EChatMode::ECM_FriendlyTeam)
				{
					LBOwnerController->ServerSendChatTextToSameTeam(PlayerState->GetPlayerName(), Text, ChatModeType);
				}
			}
		}
	}
	ExitChatEdit();

	// 로비에서 생성됨
	if (OwnerHUD->IsA<ALobbyHUD>())
	{
		OwnerController->SetInputMode(FInputModeGameAndUI());
	}
	// 게임 내에서 생성됨
	else if (OwnerHUD->IsA<ALBlasterHUD>())
	{
		OwnerController->SetInputMode(FInputModeGameOnly());
	}
}
