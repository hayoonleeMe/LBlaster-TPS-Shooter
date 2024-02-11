// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"

#include "Interface/LBCharacterWeaponInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	ADSMultiplier = 0.5f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippingWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::SetAiming(bool bInAiming)
{
	if (!EquippingWeapon)
	{
		return;
	}
	
	bIsAiming = bInAiming;
	if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
	{
		Interface->SetADSWalkSpeed(bInAiming, ADSMultiplier);
	}
	
	ServerSetAiming(bInAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bInAiming)
{
	bIsAiming = bInAiming;
	if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
	{
		Interface->SetADSWalkSpeed(bInAiming, ADSMultiplier);
	}
}

void UCombatComponent::SetFiring(bool bInFiring)
{
	if (!EquippingWeapon)
	{
		return;
	}
	
	bIsFiring = bInFiring;
	if (bIsFiring)
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		ServerFire(HitResult.ImpactPoint);
	}
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	// Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Viewport 정중앙의 크로스헤어 위치 계산 (Viewport space = screen space)
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	// Crosshair를 World Space로 변환
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility);
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippingWeapon)
	{
		if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
		{
			Interface->SetWeaponAnimLayers(EquippingWeapon->GetWeaponAnimLayer());
		}
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
	{
		Interface->PlayFireMontage(FireMontages[EquippingWeapon->GetWeaponType()]);
		EquippingWeapon->Fire(TraceHitTarget);
	}	
}

void UCombatComponent::EquipWeapon(AWeapon* InWeapon)
{
	// From ServerRPC (Server Only)
	EquippingWeapon = InWeapon;

	if (EquippingWeapon)
	{
		EquippingWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		if (ILBCharacterWeaponInterface* Interface = Cast<ILBCharacterWeaponInterface>(GetOwner()))
		{
			Interface->AttachWeapon(EquippingWeapon);
			Interface->SetWeaponAnimLayers(EquippingWeapon->GetWeaponAnimLayer());
		}
	}
}

