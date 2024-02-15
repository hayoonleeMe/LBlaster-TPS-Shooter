// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/HealthComponent.h"

#include "GameFramework/Character.h"
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

void UHealthComponent::ReceiveDamage(float InDamage)
{
	Health = FMath::Clamp(Health - InDamage, 0.f, MaxHealth);
	UpdateHUDHealth();
}

void UHealthComponent::UpdateHUDHealth() const
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(OwnerCharacter->Controller))
		{
			PlayerController->SetHUDHealth(Health, MaxHealth);
		}
	}
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();
}

void UHealthComponent::OnRep_Health()
{
	UpdateHUDHealth();
}
