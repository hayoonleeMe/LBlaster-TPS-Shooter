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

void UChatBox::AddChatMessage(const FString& InPlayerName, const FText& InText, EChatMode InChatMode, ETeam SourceTeam)
{
	if (ScrollBox && ChatEntryClass && IsValidOwnerController())
	{
		if (UChatEntry* ChatEntry = CreateWidget<UChatEntry>(OwnerController, ChatEntryClass))
		{
			if (InChatMode == EChatMode::ECM_All)
			{
				if (ALBlasterPlayerState* LBPlayerState = OwnerController->GetPlayerState<ALBlasterPlayerState>())
				{
					const bool bFriendlyTeam = LBPlayerState->GetTeam() == SourceTeam;
					const FString& TextStyleToUse = bFriendlyTeam ? UChatEntry::FriendlyTeamTextStyle : UChatEntry::OpponentTeamTextStyle;
					ChatEntry->SetChatEntryText(TEXT("[전체]"), InPlayerName, InText, TextStyleToUse);
				}
			}
			else if (InChatMode == EChatMode::ECM_FriendlyTeam)
			{
				ChatEntry->SetChatEntryText(FString(), InPlayerName, InText, UChatEntry::FriendlyTeamTextStyle);
			}
			else if (InChatMode == EChatMode::ECM_FreeForAll)
			{
				// TODO : 개인전 구현 후 다시 테스트
				ChatEntry->SetChatEntryText(TEXT("[전체]"), InPlayerName, InText, UChatEntry::DefaultTextStyle);
			}
			else if (InChatMode == EChatMode::ECM_Lobby)
			{
				ChatEntry->SetChatEntryText(FString(), InPlayerName, InText, UChatEntry::DefaultTextStyle);
			}
			
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
	if (ChatModeType != EChatMode::ECM_All || ChatModeType != EChatMode::ECM_FriendlyTeam)
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
		switch (ChatModeType)
		{
		case EChatMode::ECM_All:
		case EChatMode::ECM_FreeForAll:
		case EChatMode::ECM_Lobby:
			ChatTargetText->SetText(FText::FromString(TEXT("[전체]")));
			break;

		case EChatMode::ECM_FriendlyTeam:
			ChatTargetText->SetText(FText::FromString(TEXT("[팀]")));
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
				if (ChatModeType == EChatMode::ECM_All || ChatModeType == EChatMode::ECM_FreeForAll || ChatModeType == EChatMode::ECM_Lobby)
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
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(GetParent()->TakeWidget());
		OwnerController->SetInputMode(InputMode);
	}
	// 게임 내에서 생성됨
	else if (OwnerHUD->IsA<ALBlasterHUD>())
	{
		OwnerController->SetInputMode(FInputModeGameOnly());
	}
}
