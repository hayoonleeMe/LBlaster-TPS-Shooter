// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"

#include "LBlaster.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Character/LBlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "HUD/LBlasterHUD.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LBlasterPlayerController.h"
#include "Weapon/Projectile.h"
#include "Weapon/SniperRifle.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	/* Aiming */
	ADSMultiplier = 0.5f;

	/* Ammo */
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Unarmed, 0);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Rifle, 60);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, 4);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, 30);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, 40);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, 8);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, 4);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, 4);

	MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_Unarmed, 0);
	MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_Rifle, 180);
    MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, 24);
    MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, 90);
    MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, 240);
    MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, 32);
    MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, 24);
    MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, 24);

	/* Fire */
	FireMontages.Emplace(EWeaponType::EWT_Unarmed, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_Rifle, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_RocketLauncher, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_Pistol, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_SMG, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_Shotgun, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_SniperRifle, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_GrenadeLauncher, nullptr);

	/* Reload */
	ReloadMontages.Emplace(EWeaponType::EWT_Unarmed, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_Rifle, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_RocketLauncher, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_Pistol, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_SMG, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_Shotgun, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_SniperRifle, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_GrenadeLauncher, nullptr);

	/* Weapon */
	EquipSlotType = EEquipSlot::EES_ThirdSlot;
	EquipSlots.Add(nullptr);
	EquipSlots.Add(nullptr);
	EquipSlots.Add(nullptr);
	EquipSlots.Add(nullptr);	// dummy

	EquipMontages.Emplace(EWeaponType::EWT_Unarmed, nullptr);
	EquipMontages.Emplace(EWeaponType::EWT_Rifle, nullptr);
	EquipMontages.Emplace(EWeaponType::EWT_RocketLauncher, nullptr);
	EquipMontages.Emplace(EWeaponType::EWT_Pistol, nullptr);
	EquipMontages.Emplace(EWeaponType::EWT_SMG, nullptr);
	EquipMontages.Emplace(EWeaponType::EWT_Shotgun, nullptr);
	EquipMontages.Emplace(EWeaponType::EWT_SniperRifle, nullptr);
	EquipMontages.Emplace(EWeaponType::EWT_GrenadeLauncher, nullptr);

	/* Grenade */
	MaxGrenadeAmount = 4;
	GrenadeAmount = MaxGrenadeAmount;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, ServerCarriedAmmoState);
	DOREPLIFETIME(UCombatComponent, EquipSlotType);
	DOREPLIFETIME(UCombatComponent, EquipSlots);
	DOREPLIFETIME(UCombatComponent, ServerCombatStateChangedState);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
	DOREPLIFETIME(UCombatComponent, bIsFiring);
	DOREPLIFETIME(UCombatComponent, GrenadeAmount);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Tick에서 Trace를 통해 크로스헤어 색상 설정
	FHitResult HitResult;
	TraceUnderCrosshair(HitResult);
	
	// 크로스헤어 Draw
	SetHUDCrosshair(DeltaTime);
}

void UCombatComponent::SetAiming(bool bInAiming)
{
	if (!GetEquippingWeapon() || CombatState != ECombatState::ECS_Unoccupied)
	{
		return;
	}
	
	bIsAiming = bInAiming;
	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->SetADSWalkSpeed(bInAiming, ADSMultiplier);
		OwnerCharacter->SetBlendWeight(0.f);

		// Sniper Scope
		if (OwnerCharacter->IsLocallyControlled() && GetEquippingWeapon()->GetWeaponType() == EWeaponType::EWT_SniperRifle)
		{
			ShowSniperScopeWidget(bInAiming);
		}

		if (!OwnerCharacter->HasAuthority())
		{
			ServerSetAiming(bInAiming);
		}

		// Client-Side Prediction for Aiming
		if (OwnerCharacter->IsLocallyControlled())
		{
			bDesiredIsAiming = bIsAiming;
		}
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bInAiming)
{
	bIsAiming = bInAiming;
	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->SetADSWalkSpeed(bInAiming, ADSMultiplier);
	}
}

void UCombatComponent::SetFiring(bool bInFiring)
{
	if (!GetEquippingWeapon() || (bInFiring && !bCanFire))
	{
		return;
	}
	
	bIsFiring = bInFiring;
	ServerSetFiring(bInFiring);
	Fire();
}

UAnimMontage* UCombatComponent::SelectHitReactMontage(const FVector& HitNormal)
{
	const FVector& ActorForward = GetOwner()->GetActorForwardVector();
	const FVector& ActorRight = GetOwner()->GetActorRightVector();

	const float ForwardHit = FVector::DotProduct(ActorForward, HitNormal);
	const float RightHit = FVector::DotProduct(ActorRight, HitNormal);

	if (UKismetMathLibrary::InRange_FloatFloat(RightHit, -0.5f, 0.5f))
	{
		if (ForwardHit > 0.f)
		{
			return FrontHitReact[FMath::RandRange(0, FrontHitReact.Num() - 1)];
		}
		else
		{
			return BackHitReact[FMath::RandRange(0, BackHitReact.Num() - 1)];
		}
	}
	else
	{
		if (RightHit > 0.f)
		{
			return RightHitReact[FMath::RandRange(0, RightHitReact.Num() - 1)];
		}
		else
		{
			return LeftHitReact[FMath::RandRange(0, LeftHitReact.Num() - 1)];
		}
	}
}

UAnimMontage* UCombatComponent::SelectDeathMontage(const FVector& HitNormal)
{
	const FVector& ActorForward = GetOwner()->GetActorForwardVector();
	const FVector& ActorRight = GetOwner()->GetActorRightVector();

	const float ForwardHit = FVector::DotProduct(ActorForward, HitNormal);
	const float RightHit = FVector::DotProduct(ActorRight, HitNormal);

	if (UKismetMathLibrary::InRange_FloatFloat(RightHit, -0.5f, 0.5f))
	{
		if (ForwardHit > 0.f)
		{
			return FrontDeath[FMath::RandRange(0, FrontDeath.Num() - 1)];
		}
		else
		{
			return BackDeath[FMath::RandRange(0, BackDeath.Num() - 1)];
		}
	}
	else
	{
		if (RightHit > 0.f)
		{
			return RightDeath[FMath::RandRange(0, RightDeath.Num() - 1)];
		}
		else
		{
			return LeftDeath[FMath::RandRange(0, LeftDeath.Num() - 1)];
		}
	}
}

UAnimMontage* UCombatComponent::SelectReloadMontage()
{
	if (!GetEquippingWeapon() || !ReloadMontages.Contains(GetEquippingWeapon()->GetWeaponType()))
	{
		return nullptr;
	}
	return ReloadMontages[GetEquippingWeapon()->GetWeaponType()];
}

UAnimMontage* UCombatComponent::GetEquipMontage(EWeaponType InWeaponType)
{
	if (EquipMontages.Contains(InWeaponType))
	{
		return EquipMontages[InWeaponType];
	}
	return nullptr;
}

void UCombatComponent::DropWeapon()
{
	if (GetEquippingWeapon())
	{
		GetEquippingWeapon()->Dropped();
		SetEquippingWeapon(nullptr);
	}
}

void UCombatComponent::ElimWeapon()
{
	// Default Weapon을 제외한 착용 중인 무기는 Drop, 나머지는 Destroy
	if (EquipSlotType != EEquipSlot::EES_ThirdSlot)
	{
		DropWeapon();
	}
	for (int8 Index = 0; Index < EquipSlots.Num() - 1; ++Index)
	{
		if (AWeapon* WeaponInSlot = EquipSlots[Index])
		{
			WeaponInSlot->Destroy();
			EquipSlots[Index] = nullptr;
		}
	}
}

void UCombatComponent::Reload()
{
	if (GetEquippingWeapon() && GetEquippingWeapon()->NeedReload() && CombatState == ECombatState::ECS_Unoccupied)
	{
		if (CarriedAmmoMap.Contains(GetEquippingWeapon()->GetWeaponType()) && CarriedAmmoMap[GetEquippingWeapon()->GetWeaponType()] > 0)
		{
			ChangeCombatState(ECombatState::ECS_Reloading);
			HandleReload();	// Locally Controlled 캐릭터에서 바로 Reload Montage 재생
			ServerReload();	
		}
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	ChangeCombatState(ECombatState::ECS_Reloading);

	// Locally Controlled 캐릭터의 Reload Montage 중복 재생 방지
	if (IsValidOwnerCharacter() && !OwnerCharacter->IsLocallyControlled())
	{
		HandleReload();
	}
}

void UCombatComponent::HandleReload()
{
	if (IsValidOwnerCharacter())
	{
		MulticastInterruptMontage();
		
		OwnerCharacter->PlayReloadMontage(SelectReloadMontage());
	}
}

void UCombatComponent::ReloadFinished()
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}

	ChangeCombatState(ECombatState::ECS_Unoccupied);

	UpdateAmmoValues();

	if (OwnerCharacter->IsLocallyControlled() && bIsFiring)
	{
		Fire();
	}
	else if (bDelayedFire)
	{
		bDelayedFire = false;
		if (GetEquippingWeapon()->GetWeaponType() == EWeaponType::EWT_Shotgun)
		{
			ServerShotgunFire(CachedHitTargets, bCachedEnabledSSR);
		}
		else
		{
			ServerFire(CachedHitTarget, bCachedEnabledSSR);
		}
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if (!GetEquippingWeapon())
	{
		return;
	}

	const int32 ReloadAmount = AmountToReload();
	GetEquippingWeapon()->AddAmmo(ReloadAmount);
	AddCarriedAmmo(GetEquippingWeapon()->GetWeaponType(), -ReloadAmount);
}

int32 UCombatComponent::AmountToReload()
{
	if (GetEquippingWeapon())
	{
		const int32 RoomInMag = GetEquippingWeapon()->GetRoomInMag();
		if (CarriedAmmoMap.Contains(GetEquippingWeapon()->GetWeaponType()))
		{
			return FMath::Min(CarriedAmmoMap[GetEquippingWeapon()->GetWeaponType()], RoomInMag);
		}
	}
	return 0;
}

void UCombatComponent::ChangeCombatState(ECombatState InCombatStateToChange)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}

	FCombatStateChange CombatStateChange = CreateCombatStateChange(InCombatStateToChange);
	if (OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		ProcessChangeCombatState(InCombatStateToChange);
		UnacknowledgedCombatStateChanges.Add(CombatStateChange);
		ServerSendCombatStateChange(CombatStateChange);
	}
	if (OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled())
	{
		ServerSendCombatStateChange(CombatStateChange);	
	}
}

void UCombatComponent::ProcessChangeCombatState(ECombatState InCombatStateToChange)
{
	CombatState = InCombatStateToChange;
}

void UCombatComponent::OnRep_ServerCombatStateChangedState()
{
	ClearAcknowledgedCombatStateChanges(ServerCombatStateChangedState.LastCombatStateChange);

	// Unacknowledged Combat State Change 다시 적용
	if (UnacknowledgedCombatStateChanges.IsValidIndex(UnacknowledgedCombatStateChanges.Num() - 1))
	{
		CombatState = UnacknowledgedCombatStateChanges[UnacknowledgedCombatStateChanges.Num() - 1].CombatStateToChange;
	}

	if (IsValidOwnerCharacter() && OwnerCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	{
		ProcessChangeCombatState(ServerCombatStateChangedState.LastCombatStateChange.CombatStateToChange);
	
		switch (CombatState)
		{
		case ECombatState::ECS_Reloading:
			HandleReload();	
			break;
		
		case ECombatState::ECS_TossingGrenade:
			HandleUnEquipBeforeTossGrenade();
			break;
		}
	}
}

FCombatStateChange UCombatComponent::CreateCombatStateChange(ECombatState InCombatStateToChange)
{
	if (AGameStateBase* GameStateBase = GetWorld()->GetGameState())
	{
		FCombatStateChange CombatStateChange;
		CombatStateChange.CombatStateToChange = InCombatStateToChange;
		CombatStateChange.Time = GameStateBase->GetServerWorldTimeSeconds();
		return CombatStateChange;
	}
	return FCombatStateChange();
}

void UCombatComponent::ServerSendCombatStateChange_Implementation(const FCombatStateChange& InCombatStateChange)
{
	ProcessChangeCombatState(InCombatStateChange.CombatStateToChange);

	ServerCombatStateChangedState.CombatState = CombatState;
	ServerCombatStateChangedState.LastCombatStateChange = InCombatStateChange;
}

void UCombatComponent::ClearAcknowledgedCombatStateChanges(const FCombatStateChange& LastCombatStateChange)
{
	TArray<FCombatStateChange> NewArray;

	for (const FCombatStateChange& CombatStateChange : UnacknowledgedCombatStateChanges)
	{
		if (CombatStateChange.Time > LastCombatStateChange.Time)
		{
			NewArray.Add(CombatStateChange);
		}
	}
	UnacknowledgedCombatStateChanges = NewArray;
}

void UCombatComponent::MulticastInterruptMontage_Implementation()
{
	if (CombatState != ECombatState::ECS_Unoccupied)
	{
		if (IsValidOwnerCharacter())
		{
			if (OwnerCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
			{
				OwnerCharacter->GetMesh()->GetAnimInstance()->StopAllMontages(0.1f);
			}
		}

		ShowWeapon();
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

bool UCombatComponent::IsValidOwnerCharacter()
{
	if (!OwnerCharacter && GetOwner())
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(GetOwner());
	}
	return OwnerCharacter != nullptr;
}

bool UCombatComponent::IsValidOwnerController()
{
	if (!OwnerController && IsValidOwnerCharacter() && OwnerCharacter->Controller)
	{
		OwnerController = Cast<ALBlasterPlayerController>(OwnerCharacter->Controller);
	}
	return OwnerController != nullptr;
}

bool UCombatComponent::IsValidHUD()
{
	if (!HUD && IsValidOwnerController() && OwnerController->GetHUD())
	{
		HUD = Cast<ALBlasterHUD>(OwnerController->GetHUD());
	}
	return HUD != nullptr;
}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	if (!GetEquippingWeapon())
	{
		return;
	}
	
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
	
	if (UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation,CrosshairWorldPosition, CrosshairWorldDirection))
	{
		FVector Start = CrosshairWorldPosition;
		if (IsValidOwnerCharacter())
		{
			const float DistanceToCharacter = (OwnerCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 50.f);
		}
		const FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility);
		
		// HitTarget 보정
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
		// ImpactPoint Caching
		TraceHitTarget = TraceHitResult.ImpactPoint;

		// 캐릭터 조준 시 크로스 헤어 색상 변경
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->IsA(ALBlasterCharacter::StaticClass()))
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
	}
}

void UCombatComponent::StartDryFireTimer()
{
	if (!IsValidOwnerCharacter() || !GetEquippingWeapon())
	{
		return;
	}

	bCanFire = false;

	FTimerHandle Timer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([&]() { bCanFire = true; }), GetEquippingWeapon()->GetFireDelay(), false);
}

bool UCombatComponent::CanDryFire()
{
	// 탄약 부족으로 발사할 수 없는 상태
	return GetEquippingWeapon() != nullptr && bCanFire && bIsFiring && CombatState == ECombatState::ECS_Unoccupied && GetEquippingWeapon()->IsAmmoEmpty() && CarriedAmmoMap[GetEquippingWeapon()->GetWeaponType()] == 0;
}

void UCombatComponent::DryFire()
{
	if (USoundBase* DryFireSound = GetEquippingWeapon()->GetDryFireSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, GetEquippingWeapon()->GetActorLocation());
	}
	StartDryFireTimer();
}

void UCombatComponent::SetHUDCrosshair(float DeltaTime)
{
	if (!IsValidOwnerCharacter() || !IsValidOwnerController() || !IsValidHUD())
	{
		return;
	}

	if (GetEquippingWeapon())
	{
		HUDPackage.TopCrosshair = GetEquippingWeapon()->TopCrosshair;
		HUDPackage.BottomCrosshair = GetEquippingWeapon()->BottomCrosshair;
		HUDPackage.LeftCrosshair = GetEquippingWeapon()->LeftCrosshair;
		HUDPackage.RightCrosshair = GetEquippingWeapon()->RightCrosshair;
		HUDPackage.CenterCrosshair = GetEquippingWeapon()->CenterCrosshair;
	}
	else
	{
		// early return for unarmed state
		HUDPackage.TopCrosshair = nullptr;
		HUDPackage.BottomCrosshair = nullptr;
		HUDPackage.LeftCrosshair = nullptr;
		HUDPackage.RightCrosshair = nullptr;
		HUDPackage.CenterCrosshair = nullptr;
		HUD->SetHUDPackage(HUDPackage);
		return;
	}

	// 이동 속도에 따른 Crosshair Spread
	if (OwnerCharacter->GetCharacterMovement())
	{
		const FVector2D WalkSpeedRange(0.f, OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed);
		const FVector2D ClampRange(0.f, 1.f);
		const FVector Velocity = OwnerCharacter->GetCharacterMovement()->Velocity;
		const float CrosshairSpreadVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, ClampRange, Velocity.Size2D());

		if (OwnerCharacter->GetCharacterMovement()->IsFalling())
		{
			CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
		}
		else
		{
			CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
		}

		if (bIsAiming)
		{
			CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
		}
		else
		{
			CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
		}

		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 30.f);

		HUDPackage.CrosshairSpread = 0.5f + CrosshairSpreadVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
	}
	HUD->SetHUDPackage(HUDPackage);
}

void UCombatComponent::StartFireTimer()
{
	if (!IsValidOwnerCharacter() || !GetEquippingWeapon())
	{
		return;
	}

	OwnerCharacter->GetWorldTimerManager().SetTimer(FireTimer, this, &ThisClass::FireTimerFinished, GetEquippingWeapon()->GetFireDelay());
}

void UCombatComponent::FireTimerFinished()
{
	if (!GetEquippingWeapon())
	{
		return;
	}

	bCanFire = true;
	if (bIsFiring && GetEquippingWeapon()->IsAutomatic())
	{
		Fire();
	}

	if (GetEquippingWeapon()->IsAmmoEmpty())
	{
		Reload();
	}
}

void UCombatComponent::AddCarriedAmmo(EWeaponType InWeaponTypeToAdd, int32 InCarriedAmmoToAdd)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}

	FCarriedAmmoChange CarriedAmmoChange = CreateCarriedAmmoChange(InWeaponTypeToAdd, InCarriedAmmoToAdd);
	if (OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		ProcessAddCarriedAmmo(InWeaponTypeToAdd, InCarriedAmmoToAdd);
		UnacknowledgedCarriedAmmoChanges.Add(CarriedAmmoChange);
		ServerSendCarriedAmmoChange(CarriedAmmoChange);
	}
	if (OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled())
	{
		ServerSendCarriedAmmoChange(CarriedAmmoChange);
	}
}

void UCombatComponent::ProcessAddCarriedAmmo(EWeaponType InWeaponTypeToAdd, int32 InCarriedAmmoToAdd)
{
	if (CarriedAmmoMap.Contains(InWeaponTypeToAdd) && MaxCarriedAmmoMap.Contains(InWeaponTypeToAdd))
	{
		CarriedAmmoMap[InWeaponTypeToAdd] = FMath::Clamp(CarriedAmmoMap[InWeaponTypeToAdd] + InCarriedAmmoToAdd, 0, MaxCarriedAmmoMap[InWeaponTypeToAdd]);
		
		// 현재 장착중인 무기의 Carried Ammo가 업데이트됨
		if (GetEquippingWeapon() && IsValidOwnerController())
		{
			const EWeaponType WeaponType = GetEquippingWeapon()->GetWeaponType();
			if (WeaponType == InWeaponTypeToAdd)
			{
				OwnerController->SetHUDCarriedAmmo(CarriedAmmoMap[WeaponType]);
			}	
		}
	}
}

FCarriedAmmoChange UCombatComponent::CreateCarriedAmmoChange(EWeaponType InWeaponType, int32 InCarriedAmmoToAdd)
{
	if (AGameStateBase* GameStateBase = GetWorld()->GetGameState())
	{
		FCarriedAmmoChange CarriedAmmoChange;
		CarriedAmmoChange.WeaponType = InWeaponType;
		CarriedAmmoChange.CarriedAmmoToAdd = InCarriedAmmoToAdd;
		CarriedAmmoChange.Time = GameStateBase->GetServerWorldTimeSeconds();
		return CarriedAmmoChange;
	}
	return FCarriedAmmoChange();
}

void UCombatComponent::ClearAcknowledgedCarriedAmmoChanges(const FCarriedAmmoChange& LastCarriedAmmoChange)
{
	TArray<FCarriedAmmoChange> NewArray;

	for (const FCarriedAmmoChange& CarriedAmmoChange : UnacknowledgedCarriedAmmoChanges)
	{
		if (CarriedAmmoChange.Time > LastCarriedAmmoChange.Time)
		{
			NewArray.Add(CarriedAmmoChange);
		}
	}
	UnacknowledgedCarriedAmmoChanges = NewArray;
}

void UCombatComponent::OnRep_ServerCarriedAmmoState()
{
	if (!CarriedAmmoMap.Contains(ServerCarriedAmmoState.WeaponType) || !MaxCarriedAmmoMap.Contains(ServerCarriedAmmoState.WeaponType))
	{
		return;
	}
	CarriedAmmoMap[ServerCarriedAmmoState.WeaponType] = ServerCarriedAmmoState.CarriedAmmo;
	
	ClearAcknowledgedCarriedAmmoChanges(ServerCarriedAmmoState.LastCarriedAmmoChange);

	// Unacknowledged Carried Ammo Change 다시 적용
	for (const FCarriedAmmoChange& CarriedAmmoChange : UnacknowledgedCarriedAmmoChanges)
	{
		CarriedAmmoMap[CarriedAmmoChange.WeaponType] = FMath::Clamp(CarriedAmmoMap[CarriedAmmoChange.WeaponType] + CarriedAmmoChange.CarriedAmmoToAdd, 0, MaxCarriedAmmoMap[CarriedAmmoChange.WeaponType]);

		if (GetEquippingWeapon() && IsValidOwnerController())
		{
			const EWeaponType WeaponType = GetEquippingWeapon()->GetWeaponType();
			if (WeaponType == ServerCarriedAmmoState.WeaponType)
			{
				OwnerController->SetHUDCarriedAmmo(CarriedAmmoMap[WeaponType]);
			}	
		}
	}
	
	if (IsValidOwnerCharacter() && OwnerCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	{
		ProcessAddCarriedAmmo(ServerCarriedAmmoState.WeaponType, ServerCarriedAmmoState.LastCarriedAmmoChange.CarriedAmmoToAdd);
	}
}

void UCombatComponent::ServerSendCarriedAmmoChange_Implementation(const FCarriedAmmoChange& InCarriedAmmoChange)
{
	ProcessAddCarriedAmmo(InCarriedAmmoChange.WeaponType, InCarriedAmmoChange.CarriedAmmoToAdd);
	
	if (CarriedAmmoMap.Contains(InCarriedAmmoChange.WeaponType))
	{
		ServerCarriedAmmoState.WeaponType = InCarriedAmmoChange.WeaponType;
		ServerCarriedAmmoState.CarriedAmmo = CarriedAmmoMap[InCarriedAmmoChange.WeaponType];
		ServerCarriedAmmoState.LastCarriedAmmoChange = InCarriedAmmoChange;
	}
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& HitTarget)
{
	MulticastLaunchGrenade(HitTarget);
}

void UCombatComponent::UpdateHUDGrenadeAmount()
{
	if (IsValidOwnerController())
	{
		OwnerController->UpdateHUDGrenadeAmount(GrenadeAmount);
	}
}

void UCombatComponent::PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount)
{
	if (!IsValidOwnerController())
	{
		return;
	}
	
	AddCarriedAmmo(InWeaponType, InAmmoAmount);

	if (GetEquippingWeapon() && GetEquippingWeapon()->GetWeaponType() == InWeaponType && GetEquippingWeapon()->IsAmmoEmpty())
	{
		Reload();
	}
}

void UCombatComponent::ShowWeapon()
{
	// 무기를 변경할 때 숨겼던 무기를 다시 보이게 설정
	if (GetEquippingWeapon())
	{
		GetEquippingWeapon()->SetActorEnableCollision(true);
		GetEquippingWeapon()->SetActorHiddenInGame(false);
	}
}

void UCombatComponent::ServerChooseWeaponSlot_Implementation(EEquipSlot InEquipSlotType)
{
	// 슬롯 바꾸기 전에 들고 있던 무기 손에서 떼야함.
	HolsterWeapon();

	EEquipSlot PrevType = EquipSlotType;
	
	// 바꾼 슬롯에 있는 무기 장착
	EquipSlotType = InEquipSlotType;
	if (GetEquippingWeapon())
	{
		EquipWeapon(GetEquippingWeapon());
	}
	// 슬롯에 무기가 없으므로 Unarmed
	else
	{
		bool bSkipUnEquipMontage = false;
		if (!EquipSlots[static_cast<uint8>(PrevType)])
		{
			bSkipUnEquipMontage = true;
		}
		MulticastSwitchToUnarmedState(bSkipUnEquipMontage);
	}
}

void UCombatComponent::ChooseWeaponSlot(EEquipSlot InEquipSlotType)
{
	if (EquipSlotType == InEquipSlotType)
	{
		return;
	}
	
	SetAiming(false);
	SetFiring(false);
	bCanFire = true;
	ServerChooseWeaponSlot(InEquipSlotType);
	EquipSlotType = InEquipSlotType;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValidOwnerCharacter())
	{
		return;
	}

	if (!OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled())
	{
		OwnerCharacter->UpdateHUDHealth();
		UpdateHUDGrenadeAmount();
		ServerEquipDefaultWeapon();
	}
}

void UCombatComponent::ShowAttachedGrenade(bool bShow)
{
	if (IsValidOwnerCharacter() && OwnerCharacter->GetAttachedGrenade())
	{
		OwnerCharacter->GetAttachedGrenade()->SetVisibility(bShow);
	}
}

void UCombatComponent::ServerTossGrenade_Implementation()
{
	// 무기 숨김
	GetEquippingWeapon()->SetActorEnableCollision(false);
	GetEquippingWeapon()->SetActorHiddenInGame(true);
	
	ChangeCombatState(ECombatState::ECS_TossingGrenade);
	GrenadeAmount = FMath::Clamp(GrenadeAmount - 1, 0, MaxGrenadeAmount);
	HandleUnEquipBeforeTossGrenade();
}

void UCombatComponent::MulticastLaunchGrenade_Implementation(const FVector_NetQuantize& HitTarget)
{
	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	LocalLaunchGrenade(HitTarget);
}

void UCombatComponent::LocalLaunchGrenade(const FVector_NetQuantize& HitTarget)
{
	if (IsValidOwnerCharacter() && OwnerCharacter->GetAttachedGrenade() && GrenadeClass)
	{
		const FVector StartingLocation = OwnerCharacter->GetAttachedGrenade()->GetComponentLocation();
		const FVector ToTarget = HitTarget - StartingLocation;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = SpawnParams.Instigator = OwnerCharacter;

		if (UWorld* World = GetWorld())
		{
			AProjectile* Projectile = World->SpawnActor<AProjectile>(GrenadeClass, StartingLocation, ToTarget.Rotation(), SpawnParams);
			
			if (OwnerCharacter->HasAuthority())
			{
				Projectile->SetReplicatesPostInit(false);
			}
		}
	}
}

void UCombatComponent::HandleTossGrenade()
{
	if (IsValidOwnerCharacter() && TossGrenadeMontage)
	{
		OwnerCharacter->PlayTossGrenadeMontage(TossGrenadeMontage);
		ShowAttachedGrenade(true);
		UpdateHUDGrenadeAmount();
	}
}

void UCombatComponent::HandleUnEquipBeforeTossGrenade()
{
	if (IsValidOwnerCharacter() && UnEquipBeforeTossGrenadeMontage)
	{
		OwnerCharacter->PlayTossGrenadeMontage(UnEquipBeforeTossGrenadeMontage);
	}
}

void UCombatComponent::StartTossGrenade()
{
	HandleTossGrenade();
}

void UCombatComponent::EquipFinished()
{
	ChangeCombatState(ECombatState::ECS_Unoccupied);
}

void UCombatComponent::InitSniperScope()
{
	if (IsValidHUD() && GetEquippingWeapon() && GetEquippingWeapon()->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		if (const ASniperRifle* SniperRifle = Cast<ASniperRifle>(GetEquippingWeapon()))
		{
			if (SniperRifle->GetSniperScopeClass())
			{
				HUD->InitSniperScope(SniperRifle->GetSniperScopeClass());
			}
		}
	}
}

void UCombatComponent::ShowSniperScopeWidget(bool bShowScope)
{
	if (IsValidHUD() && GetEquippingWeapon() && GetEquippingWeapon()->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		if (USoundBase* ZoomSound = bShowScope ? SniperScopeZoomInSound : SniperScopeZoomOutSound)
		{
			UGameplayStatics::PlaySound2D(this, ZoomSound);
		}
		
		HUD->ShowSniperScopeWidget(bShowScope);
	}
}

void UCombatComponent::TossGrenade()
{
	if (GetEquippingWeapon() && CombatState == ECombatState::ECS_Unoccupied && GrenadeAmount > 0)
	{
		ChangeCombatState(ECombatState::ECS_TossingGrenade);
		ServerTossGrenade();
	}
}

void UCombatComponent::TossGrenadeFinished()
{
	
	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->PlayEquipMontage(GetEquipMontage(GetEquippingWeapon()->GetWeaponType()));
	}
	ChangeCombatState(ECombatState::ECS_Unoccupied);

	const EWeaponType WeaponType = GetEquippingWeapon() ? GetEquippingWeapon()->GetWeaponType() : EWeaponType::EWT_Unarmed;
	HandleEquip(WeaponType);
}

void UCombatComponent::LaunchGrenade()
{
	ShowAttachedGrenade(false);

	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled())
	{
		LocalLaunchGrenade(TraceHitTarget);
		ServerLaunchGrenade(TraceHitTarget);	
	}
}

void UCombatComponent::ServerSetFiring_Implementation(bool bInFiring)
{
	bIsFiring = bInFiring;
}

bool UCombatComponent::CanFire()
{
	return GetEquippingWeapon() != nullptr && !GetEquippingWeapon()->IsAmmoEmpty() && bCanFire && bIsFiring && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::Fire()
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}
	
	if (CanFire())
	{
		bCanFire = false;

		if (GetEquippingWeapon()->GetWeaponType() == EWeaponType::EWT_Shotgun)
		{
			// Shotgun은 Scatter를 적용한 Pellet 개수만큼의 TraceHitTargets를 사용
			const TArray<FVector_NetQuantize>& TraceHitTargets = GetEquippingWeapon()->ShotgunTraceEndWithScatter(TraceHitTarget);

			ShotgunLocalFire(TraceHitTargets);
			ServerShotgunFire(TraceHitTargets, OwnerCharacter->IsServerSideRewindEnabled());
		}
		else
		{
			// Scatter가 적용된 TraceHitTarget로 업데이트
			if (GetEquippingWeapon()->DoesUseScatter())
			{
				TraceHitTarget = GetEquippingWeapon()->TraceEndWithScatter(TraceHitTarget);
			}
		
			// Fire Montage등 cosmetic effect는 로컬에서 먼저 수행
			LocalFire(TraceHitTarget);	
			ServerFire(TraceHitTarget, OwnerCharacter->IsServerSideRewindEnabled());
		}
		
		CrosshairShootingFactor = 0.75f;
		StartFireTimer();

		// 수직 반동
		OwnerCharacter->AddControllerPitchInput(-1.f * GetEquippingWeapon()->GetVerticalRecoilValue());
	}
	else if (CanDryFire())
	{
		DryFire();
	}
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& HitTarget)
{
	if (IsValidOwnerCharacter() && GetEquippingWeapon())
	{
		if (UAnimMontage* MontageToPlay = FireMontages[GetEquippingWeapon()->GetWeaponType()])
		{
			OwnerCharacter->PlayFireMontage(MontageToPlay);
		}
		GetEquippingWeapon()->Fire(HitTarget);
	}
}

void UCombatComponent::ShotgunLocalFire(const TArray<FVector_NetQuantize>& HitTargets)
{
	if (IsValidOwnerCharacter() && GetEquippingWeapon())
	{
		if (UAnimMontage* MontageToPlay = FireMontages[GetEquippingWeapon()->GetWeaponType()])
		{
			OwnerCharacter->PlayFireMontage(MontageToPlay);
		}
		GetEquippingWeapon()->ShotgunFire(HitTargets);
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& HitTarget, bool bEnabledSSR)
{
	// 만약 서버에서 아직 장전 중인데 클라에서 Fire RPC가 호출된 경우
	if (CombatState == ECombatState::ECS_Reloading)
	{
		bDelayedFire = true;
		CachedHitTarget = HitTarget;
		bCachedEnabledSSR = bEnabledSSR;
		return;
	}

	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->EnableServerSideRewind(bEnabledSSR);
	}
	MulticastFire(HitTarget);	
}

void UCombatComponent::ServerShotgunFire_Implementation(const TArray<FVector_NetQuantize>& HitTargets, bool bEnabledSSR)
{
	// 만약 서버에서 아직 장전 중인데 클라에서 Fire RPC가 호출된 경우
	if (CombatState == ECombatState::ECS_Reloading)
	{
		bDelayedFire = true;
		CachedHitTargets = HitTargets;
		bCachedEnabledSSR = bEnabledSSR;
		return;
	}
	
	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->EnableServerSideRewind(bEnabledSSR);
	}
	MulticastShotgunFire(HitTargets);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& HitTarget)
{
	// Locally Controlled Character의 Local Fire 중복 호출 방지
	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	LocalFire(HitTarget);
}

void UCombatComponent::MulticastShotgunFire_Implementation(const TArray<FVector_NetQuantize>& HitTargets)
{
	// Locally Controlled Character의 Local Fire 중복 호출 방지
	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	ShotgunLocalFire(HitTargets);
}

FString UCombatComponent::GetWeaponTypeString (EWeaponType InWeaponType)
{
	if (InWeaponType == EWeaponType::EWT_Rifle)
	{
		return FString(TEXT("Assault Rifle"));
	}
	if (InWeaponType == EWeaponType::EWT_RocketLauncher)
	{
		return FString(TEXT("Rocket Launcher"));
	}
	if (InWeaponType == EWeaponType::EWT_Pistol)
	{
		return FString(TEXT("Pistol"));
	}
	if (InWeaponType == EWeaponType::EWT_SMG)
	{
		return FString(TEXT("SMG"));
	}
	if (InWeaponType == EWeaponType::EWT_Shotgun)
	{
		return FString(TEXT("Shotgun"));
	}
	if (InWeaponType == EWeaponType::EWT_SniperRifle)
	{
		return FString(TEXT("Sniper Rifle"));
	}
	if (InWeaponType == EWeaponType::EWT_GrenadeLauncher)
	{
		return FString(TEXT("Grenade Launcher"));
	}
	return FString();
}

void UCombatComponent::AttachWeapon()
{
	if (IsValidOwnerCharacter() && GetEquippingWeapon())
	{
		if (USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh())
		{
			GetEquippingWeapon()->SetActorRelativeTransform(GetEquippingWeapon()->GetAttachTransform());
			GetEquippingWeapon()->AttachToComponent(OwnerMesh, FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("weapon_r")));
		}
	}
}

void UCombatComponent::HandleEquip(EWeaponType InWeaponType)
{
	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->PlayEquipMontage(GetEquipMontage(InWeaponType));
	}
}

void UCombatComponent::SetEquippingWeapon(AWeapon* InWeapon)
{
	EquipSlots[static_cast<uint8>(EquipSlotType)] = InWeapon;

	// Rep Notify 호출을 위한 dummy
	EquipSlots[static_cast<uint8>(EEquipSlot::EES_MAX)] = EquipSlots[static_cast<uint8>(EEquipSlot::EES_MAX)] ? nullptr : InWeapon;
}

void UCombatComponent::OnRep_IsAiming()
{
	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled())
	{
		bIsAiming = bDesiredIsAiming;
	}
}

void UCombatComponent::ServerEquipDefaultWeapon_Implementation()
{
	if (EquipSlotType != EEquipSlot::EES_ThirdSlot)
	{
		return;
	}
	
	if (!GetEquippingWeapon() && DefaultWeaponClass)
	{
		if (AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass))
		{
			SetEquippingWeapon(DefaultWeapon);
			EquipWeapon(GetEquippingWeapon());
		}
	}
}

FTransform UCombatComponent::GetWeaponLeftHandTransform()
{
	if (GetEquippingWeapon())
	{
		return GetEquippingWeapon()->GetWeaponMesh()->GetSocketTransform(FName(TEXT("LeftHandSocket")), RTS_ParentBoneSpace);
	}
	return FTransform::Identity;
}

void UCombatComponent::ServerEquipOverlappingWeapon_Implementation()
{
	// 1 or 2번 슬롯일 때 비었으면 바로 착용, 안 비었으면 무기 변경
	if (IsValidOwnerCharacter() && OwnerCharacter->GetOverlappingWeapon())
	{
		// 무기 스왑
		if (GetEquippingWeapon())
		{
			SwapWeapon(OwnerCharacter->GetOverlappingWeapon());
		}
		// 새로 장착
		else
		{
			EquipWeapon(OwnerCharacter->GetOverlappingWeapon());
		}	
	}
}

void UCombatComponent::EquipOverlappingWeapon()
{
	SetAiming(false);
	SetFiring(false);
	ServerEquipOverlappingWeapon();
}

void UCombatComponent::EquipWeapon(AWeapon* InWeapon)
{
	// From ServerRPC (Server Only)
	MulticastInterruptMontage();

	SetEquippingWeapon(InWeapon);
	if (GetEquippingWeapon())
	{
		GetEquippingWeapon()->SetWeaponState(EWeaponState::EWS_Equipped);

		if (IsValidOwnerCharacter())
		{
			GetEquippingWeapon()->SetOwner(OwnerCharacter);
			GetEquippingWeapon()->SetHUDAmmo();
			
			if (CarriedAmmoMap.Contains(GetEquippingWeapon()->GetWeaponType()))
			{
				if (IsValidOwnerController() && OwnerCharacter->IsLocallyControlled())
				{
					OwnerController->SetHUDCarriedAmmo(CarriedAmmoMap[GetEquippingWeapon()->GetWeaponType()]);
					OwnerController->SetHUDWeaponTypeText(GetWeaponTypeString(GetEquippingWeapon()->GetWeaponType()));
					OwnerController->SetWeaponSlotIcon(EquipSlotType, GetEquippingWeapon()->GetWeaponType());
					OwnerController->ChooseWeaponSlot(EquipSlotType);
				}
			}
			
			AttachWeapon();
			OwnerCharacter->SetWeaponAnimLayers(GetEquippingWeapon()->GetWeaponType(), GetEquippingWeapon()->GetWeaponAnimLayer());
			HandleEquip(GetEquippingWeapon()->GetWeaponType());
			UGameplayStatics::PlaySoundAtLocation(this, GetEquippingWeapon()->GetEquipSound(), GetEquippingWeapon()->GetActorLocation());

			/* ADS FOV */
			OwnerCharacter->SetAdsFov(GetEquippingWeapon()->GetADSFOV());
			
			/* Sniper Scope */
			InitSniperScope();

			// Reload Empty Weapon
			if (OwnerCharacter->IsLocallyControlled() && GetEquippingWeapon()->IsAmmoEmpty())
			{
				Reload();
			}
		}
	}
}

void UCombatComponent::OnRep_EquipSlots()
{
	MulticastInterruptMontage();
	
	if (GetEquippingWeapon())
	{
		GetEquippingWeapon()->SetWeaponState(EWeaponState::EWS_Equipped);
		
		if (IsValidOwnerCharacter())
		{
			if (CarriedAmmoMap.Contains(GetEquippingWeapon()->GetWeaponType()))
			{
				if (IsValidOwnerController() && OwnerCharacter->IsLocallyControlled())
				{
					OwnerController->SetHUDCarriedAmmo(CarriedAmmoMap[GetEquippingWeapon()->GetWeaponType()]);
					OwnerController->SetHUDWeaponTypeText(GetWeaponTypeString(GetEquippingWeapon()->GetWeaponType()));
					OwnerController->SetWeaponSlotIcon(EquipSlotType, GetEquippingWeapon()->GetWeaponType());
					OwnerController->ChooseWeaponSlot(EquipSlotType);
				}	
			}

			AttachWeapon();
			OwnerCharacter->SetWeaponAnimLayers(GetEquippingWeapon()->GetWeaponType(), GetEquippingWeapon()->GetWeaponAnimLayer());
			HandleEquip(GetEquippingWeapon()->GetWeaponType());
			UGameplayStatics::PlaySoundAtLocation(this, GetEquippingWeapon()->GetEquipSound(), GetEquippingWeapon()->GetActorLocation());

			/* ADS FOV */
			OwnerCharacter->SetAdsFov(GetEquippingWeapon()->GetADSFOV());
			
			/* Sniper Scope */
			InitSniperScope();

			// Reload Empty Weapon
			if (GetEquippingWeapon()->IsAmmoEmpty())
			{
				Reload();
			}
		}
	}
}

void UCombatComponent::SwapWeapon(AWeapon* InWeapon)
{
	// 기존 무기 떨구고 새 무기 장착
	DropWeapon();
	EquipWeapon(InWeapon);
}

void UCombatComponent::HolsterWeapon()
{
	if (GetEquippingWeapon())
	{
		GetEquippingWeapon()->SetActorEnableCollision(false);
		GetEquippingWeapon()->SetActorHiddenInGame(true);
		GetEquippingWeapon()->Holstered();
	}
}

void UCombatComponent::MulticastSwitchToUnarmedState_Implementation(bool bSkipUnEquipMontage)
{
	if (IsValidOwnerCharacter())
	{
		if (IsValidOwnerController() && OwnerCharacter->IsLocallyControlled())
		{
			OwnerController->SetHUDAmmo(0);
			OwnerController->SetHUDCarriedAmmo(0);
			OwnerController->SetHUDWeaponTypeText(GetWeaponTypeString());
			OwnerController->SetWeaponSlotIcon(EquipSlotType, EWeaponType::EWT_Unarmed);
			OwnerController->ChooseWeaponSlot(EquipSlotType);
		}
		
		if (!bSkipUnEquipMontage)
		{
			OwnerCharacter->SetWeaponAnimLayers(EWeaponType::EWT_Unarmed);
			HandleEquip(EWeaponType::EWT_Unarmed);
		}
	}
}
