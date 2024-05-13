// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainMenuPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "HUD/MainMenuHUD.h"

AMainMenuPlayerController::AMainMenuPlayerController()
{
	/* Input */
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MenuMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/LBlaster/Core/Inputs/IMC_MenuContext.IMC_MenuContext'"));
	if (MenuMappingContextRef.Object)
	{
		MenuMappingContext = MenuMappingContextRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> ReturnMenuActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/LBlaster/Core/Inputs/IA_ReturnMenu.IA_ReturnMenu'"));
	if (ReturnMenuActionRef.Object)
	{
		ReturnMenuAction = ReturnMenuActionRef.Object;
	}
}

void AMainMenuPlayerController::BeginPlay()
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

void AMainMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	/* IMC_MenuContext */
	EnhancedInputComponent->BindAction(ReturnMenuAction, ETriggerEvent::Triggered, this, &ThisClass::ReturnMenu);
}

void AMainMenuPlayerController::ReturnMenu()
{
	if (IsValidOwningHUD())
	{
		OwningHUD->ReturnMenu();
	}
}

bool AMainMenuPlayerController::IsValidOwningHUD()
{
	if (!OwningHUD && GetHUD())
	{
		OwningHUD = Cast<AMainMenuHUD>(GetHUD());
	}
	return OwningHUD != nullptr;
}
