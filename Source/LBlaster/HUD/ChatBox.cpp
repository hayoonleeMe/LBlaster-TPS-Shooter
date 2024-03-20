// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChatBox.h"

#include "ChatEntry.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "GameFramework/PlayerState.h"
#include "Player/LBlasterPlayerController.h"

void UChatBox::FocusChatEdit() const
{
	if (ChatEditText)
	{
		ChatEditText->SetIsEnabled(true);
		ChatEditText->SetFocus();
	}
}

void UChatBox::ExitChatEdit() const
{
	if (ChatEditText)
	{
		ChatEditText->SetText(FText::GetEmpty());
		ChatEditText->SetIsEnabled(false);
	}
}

void UChatBox::AddChatMessage(const FText& InText)
{
	if (ScrollBox && ChatEntryClass && IsValidPlayerController())
	{
		if (UChatEntry* ChatEntry = CreateWidget<UChatEntry>(PlayerController, ChatEntryClass))
		{
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

void UChatBox::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatEditText)
	{
		ChatEditText->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted);
	}
}

bool UChatBox::IsValidPlayerController()
{
	if (!PlayerController && GetWorld())
	{
		PlayerController = GetWorld()->GetFirstPlayerController<ALBlasterPlayerController>();
	}
	return PlayerController != nullptr;
}

void UChatBox::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsValidPlayerController())
	{
		return;
	}
	
	if (CommitMethod == ETextCommit::OnEnter && !Text.IsEmpty())
	{
		if (APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>())
		{
			const FText ChatText = FText::FromString(FString::Printf(TEXT("%s : %s"), *PlayerState->GetPlayerName(), *Text.ToString()));
			PlayerController->ServerSendChatText(ChatText);
		}
	}
	ExitChatEdit();
	PlayerController->SetInputMode(FInputModeGameOnly());
}
