// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/HealthComponent.h"

#include "Character/LBlasterCharacter.h"
#include "GameMode/LBlasterGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Player/LBlasterPlayerController.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
	/* Health */
	MaxHealth = 100.f;
	Health = MaxHealth;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}

void UHealthComponent::ReceiveDamage(float InDamage, AController* InstigatorController)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}
	
	Health = FMath::Clamp(Health - InDamage, 0.f, MaxHealth);
	UpdateHUDHealth();

	if (Health == 0.f)
	{
		if (ALBlasterGameMode* GameMode = GetWorld()->GetAuthGameMode<ALBlasterGameMode>())
		{
			if (ALBlasterPlayerController* VictimController = Cast<ALBlasterPlayerController>(OwnerCharacter->Controller))
			{
				if (ALBlasterPlayerController* AttackerController = Cast<ALBlasterPlayerController>(InstigatorController))
				{
					GameMode->PlayerEliminated(OwnerCharacter, VictimController, AttackerController);
				}
			}	
		}
	}
}

void UHealthComponent::UpdateHUDHealth()
{
	if (IsValidOwnerCharacter())
	{
		if (ALBlasterPlayerController* OwnerController = Cast<ALBlasterPlayerController>(OwnerCharacter->Controller))
		{
			OwnerController->SetHUDHealth(Health, MaxHealth);
		}
	}
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();
}

bool UHealthComponent::IsValidOwnerCharacter()
{
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(GetOwner());
	}
	return OwnerCharacter != nullptr;
}

void UHealthComponent::OnRep_Health()
{
	UpdateHUDHealth();
}
