// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChatBox.h"

#include "ChatEntry.h"
#include "LBlasterHUD.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "GameFramework/PlayerState.h"
#include "Player/LBlasterPlayerController.h"

void UChatBox::FocusChatEdit()
{
	if (ChatEditText)
	{	
		ChatEditText->SetIsEnabled(true);
		ChatEditText->SetFocus();
	}
	if (ChatBoxFadeOut)
	{
		StopAnimation(ChatBoxFadeOut);
	}
	GetWorld()->GetTimerManager().ClearTimer(ChatBoxFadeOutTimer);
	SetRenderOpacity(1.f);
}

void UChatBox::ExitChatEdit()
{
	if (ChatEditText)
	{
		ChatEditText->SetText(FText::GetEmpty());
		ChatEditText->SetIsEnabled(false);
	}

	StartChatBoxFadeOutTimer();
}

void UChatBox::AddChatMessage(const FText& InText)
{
	if (ScrollBox && ChatEntryClass && IsValidOwnerController())
	{
		if (UChatEntry* ChatEntry = CreateWidget<UChatEntry>(OwnerController, ChatEntryClass))
		{
			ChatEntry->SetChatEntryText(InText);
			ScrollBox->AddChild(ChatEntry);
			ScrollBox->ScrollToEnd();

			if (ChatBoxFadeOut)
			{
				StopAnimation(ChatBoxFadeOut);
			}
			SetRenderOpacity(1.f);
			StartChatBoxFadeOutTimer();
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

void UChatBox::StartChatBoxFadeOutTimer()
{
	// Wait Time을 기다리고 Fade Out 애니메이션 재생
	GetWorld()->GetTimerManager().SetTimer(ChatBoxFadeOutTimer, FTimerDelegate::CreateLambda([&]()
	{
		GetWorld()->GetTimerManager().SetTimer(ChatBoxFadeOutTimer, FTimerDelegate::CreateLambda([&]()
		{
			if (ChatBoxFadeOut)
			{
				PlayAnimation(ChatBoxFadeOut);
			}
		}), ChatBoxFadeOutDuration, false);
	}), ChatBoxFadeOutWaitTime, false);
}

void UChatBox::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsValidOwnerController())
	{
		return;
	}
	
	if (CommitMethod == ETextCommit::OnEnter && !Text.IsEmpty())
	{
		if (APlayerState* PlayerState = OwnerController->GetPlayerState<APlayerState>())
		{
			const FText ChatText = FText::FromString(FString::Printf(TEXT("%s : %s"), *PlayerState->GetPlayerName(), *Text.ToString()));
			OwnerController->ServerSendChatText(ChatText);
		}
	}
	ExitChatEdit();
	OwnerController->SetInputMode(FInputModeGameOnly());
}
