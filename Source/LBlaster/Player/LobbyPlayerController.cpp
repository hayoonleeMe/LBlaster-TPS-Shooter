// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "HUD/LobbyHUD.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	/* Input */
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MenuMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/LBlaster/Core/Inputs/IMC_MenuContext.IMC_MenuContext'"));
	if (MenuMappingContextRef.Object)
	{
		MenuMappingContext = MenuMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> FocusChatActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_FocusChat.IA_FocusChat'"));
	if (FocusChatActionRef.Object)
	{
		FocusChatAction = FocusChatActionRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> ReturnMenuActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_ReturnMenu.IA_ReturnMenu'"));
	if (ReturnMenuActionRef.Object)
	{
		ReturnMenuAction = ReturnMenuActionRef.Object;
	}
}

void ALobbyPlayerController::ClientSendAddPlayerList_Implementation(ETeam InTeam, const FString& InName)
{
	if (ALobbyHUD* LobbyHUD = GetHUD<ALobbyHUD>())
	{
		LobbyHUD->AddNewPlayerForClient(InTeam, InName);
	}
}

void ALobbyPlayerController::ClientSendAddPlayerListFFA_Implementation(const FString& InName)
{
	if (ALobbyHUD* LobbyHUD = GetHUD<ALobbyHUD>())
	{
		LobbyHUD->AddNewPlayerForClient(ETeam::ET_MAX, InName);
	}
}

void ALobbyPlayerController::ClientSendTeamChangePlayerList_Implementation(ETeam CurrentTeam, ETeam NewTeam, const FString& InName)
{
	if (ALobbyHUD* LobbyHUD = GetHUD<ALobbyHUD>())
	{
		LobbyHUD->ChangePlayerTeamForClient(CurrentTeam, NewTeam, InName);
	}
}

void ALobbyPlayerController::ServerSendTeamChangePlayerList_Implementation(ETeam CurrentTeam, ETeam NewTeam, ALBlasterPlayerState* InPlayerState)
{
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		if (ALobbyHUD* LobbyHUD = GetWorld()->GetFirstPlayerController()->GetHUD<ALobbyHUD>())
		{
			LobbyHUD->ChangePlayerTeamFromClient(CurrentTeam, NewTeam, InPlayerState);
		}	
	}
}

void ALobbyPlayerController::ClientSendRemovePlayerList_Implementation(ETeam InTeam, const FString& InName)
{
	if (ALobbyHUD* LobbyHUD = GetHUD<ALobbyHUD>())
	{
		LobbyHUD->RemoveExitingPlayer(InTeam, InName, false);
	}
}

void ALobbyPlayerController::ClientSendRemovePlayerListFFA_Implementation(const FString& InName)
{
	if (ALobbyHUD* LobbyHUD = GetHUD<ALobbyHUD>())
	{
		LobbyHUD->RemoveExitingPlayer(ETeam::ET_MAX, InName, false);
	}
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/* Input */
	if (GEngine && GetWorld() && MenuMappingContext && IsLocalController())
	{
		if (const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(GetWorld()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->AddMappingContext(MenuMappingContext, 0);
			}	
		}
	}
}

void ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	/* IMC_MenuContext */
	EnhancedInputComponent->BindAction(FocusChatAction, ETriggerEvent::Triggered, this, &ThisClass::FocusChat);
	EnhancedInputComponent->BindAction(ReturnMenuAction, ETriggerEvent::Triggered, this, &ThisClass::ReturnMenu);
}

void ALobbyPlayerController::FocusChat()
{
	if (IsValidOwningHUD())
	{
		OwningHUD->FocusChat();
	}
}

void ALobbyPlayerController::ReturnMenu()
{
	if (IsValidOwningHUD())
	{
		OwningHUD->ReturnMenu(false);
	}
}

bool ALobbyPlayerController::IsValidOwningHUD()
{
	if (!OwningHUD && GetHUD())
	{
		OwningHUD = Cast<ALobbyHUD>(GetHUD());
	}
	return OwningHUD != nullptr;
}
