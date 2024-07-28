// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"

#include "LBlaster.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Character/LBlasterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "HUD/LBlasterHUD.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LBlasterPlayerController.h"
#include "Weapon/Projectile.h"
#include "Weapon/SniperRifle.h"
#include "Weapon/ThrowableGrenade.h"

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

	/* Crosshair */
	static ConstructorHelpers::FObjectFinder<UTexture2D> CenterCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Center.Crosshair_Center'"));
	if (CenterCrosshairRef.Object)
	{
		DefaultCrosshair.CenterCrosshair = CenterCrosshairRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> TopCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Top.Crosshair_Top'"));
	if (TopCrosshairRef.Object)
	{
		DefaultCrosshair.TopCrosshair = TopCrosshairRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> BottomCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Bottom.Crosshair_Bottom'"));
	if (BottomCrosshairRef.Object)
	{
		DefaultCrosshair.BottomCrosshair = BottomCrosshairRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> LeftCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Left.Crosshair_Left'"));
	if (LeftCrosshairRef.Object)
	{
		DefaultCrosshair.LeftCrosshair = LeftCrosshairRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> RightCrosshairRef(TEXT("/Script/Engine.Texture2D'/Game/LBlaster/UI/Crosshair/Primary/Crosshair_Right.Crosshair_Right'"));
	if (RightCrosshairRef.Object)
	{
		DefaultCrosshair.RightCrosshair = RightCrosshairRef.Object;
	}

	/* Auto Fire */
	bCanFire = true;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, ServerCarriedAmmoState);
	DOREPLIFETIME(UCombatComponent, ServerCombatStateChangedState);
	DOREPLIFETIME(UCombatComponent, ServerWeaponEquipState);
	DOREPLIFETIME_CONDITION(UCombatComponent, DefaultWeapon, COND_InitialOnly);
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
	
	if (bShowCrosshair)
	{
		// 크로스헤어 Draw
		UpdateHUDCrosshair(DeltaTime);
	}

	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled() && CombatState == ECombatState::ECS_TossingGrenade)
	{
		DrawGrenadeTrajectory();
	}
}

void UCombatComponent::SetAiming(bool bInAiming)
{
	if (!GetEquippingWeapon() || CombatState != ECombatState::ECS_Unoccupied || bIsAiming == bInAiming)
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

void UCombatComponent::ElimWeapon()
{
	if (AWeapon* WeaponInSlot = EquipSlots[static_cast<uint8>(EquipSlotType)])
	{
		WeaponInSlot->Destroy();
		EquipSlots[static_cast<uint8>(EquipSlotType)] = nullptr;
	}
}

UAnimMontage* UCombatComponent::GetEquipMontage(EWeaponType InWeaponType)
{
	if (EquipMontages.Contains(InWeaponType))
	{
		return EquipMontages[InWeaponType];
	}
	return nullptr;
}

void UCombatComponent::ElimAllWeapon()
{
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
			if (IsValidOwnerCharacter() && OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
			{
				ServerReload();
			}
		}
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	ChangeCombatState(ECombatState::ECS_Reloading);
}

void UCombatComponent::HandleReload()
{
	if (IsValidOwnerCharacter())
	{
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

	// Aiming이 Interrupt 됐을 때 다시 Aiming
	if (OwnerCharacter->IsLocallyControlled() && bAimingKeyPressed && !bIsAiming && GetEquippingWeapon())
	{
		SetAiming(true);
	}

	if (OwnerCharacter->IsLocallyControlled() && bIsFiring && bCanFire && GetEquippingWeapon() && GetEquippingWeapon()->IsAutomatic())
	{
		Fire();
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

void UCombatComponent::ChangeCombatState(ECombatState InCombatStateToChange, bool bPlayEquipMontage, bool bShouldPlayUnarmedEquipMontage, bool bCanSendCombatStateRPC)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}

	// 전투 관련 상태 초기화
	if (OwnerCharacter->IsLocallyControlled())
	{
		SetAiming(false);
		bCanFire = true;
		GetWorld()->GetTimerManager().ClearTimer(FireTimer);	
	}
	
	FCombatStateChange CombatStateChange = CreateCombatStateChange(InCombatStateToChange, bPlayEquipMontage, bShouldPlayUnarmedEquipMontage);
	if (OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		ProcessChangeCombatState(InCombatStateToChange, bPlayEquipMontage, bShouldPlayUnarmedEquipMontage);
		UnacknowledgedCombatStateChanges.Add(CombatStateChange);
		// Autonomous Proxy에서 EquipWeapon()에 의해 해당 함수가 호출되는 경우 Server RPC도 함께 수행하므로 여기서도 CombatState Change에 대해 Server RPC를 보낼 필요가 없다.
		if (bCanSendCombatStateRPC)
		{
			ServerSendCombatStateChange(CombatStateChange);
		}
	}

	// Autonomous Proxy에 의해 전송된 Equip Server RPC에 의해 이 함수가 호출됐을 때도 수행해줘야 한다.
	if (OwnerCharacter->HasAuthority() && (OwnerCharacter->IsLocallyControlled() || OwnerCharacter->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		ServerSendCombatStateChange(CombatStateChange);	
	}
}

void UCombatComponent::ProcessChangeCombatState(ECombatState InCombatStateToChange, bool bPlayEquipMontage, bool bShouldPlayUnarmedEquipMontage)
{
	CombatState = InCombatStateToChange;
	OnChangedCombatState(bPlayEquipMontage, bShouldPlayUnarmedEquipMontage);
}

void UCombatComponent::OnChangedCombatState(bool bPlayEquipMontage, bool bShouldPlayUnarmedEquipMontage)
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();	
		break;
		
	case ECombatState::ECS_TossingGrenade:
		// 무기 숨김
		if (GetEquippingWeapon())
		{
			GetEquippingWeapon()->SetWeaponVisibility(false);
			HandleUnEquipBeforeTossGrenade();
		}
		else
		{
			StartTossGrenade(false);
		}
		bDrawGrenadeTrajectory = true;
		break;
		
	case ECombatState::ECS_Equipping:
		// Equip Montage 재생
		if (bPlayEquipMontage)
		{
			if (bShouldPlayUnarmedEquipMontage)
			{
				HandleEquip(EWeaponType::EWT_Unarmed);
			}
			else if (GetEquippingWeapon())
			{
				HandleEquip(GetEquippingWeapon()->GetWeaponType());
				UGameplayStatics::PlaySoundAtLocation(this, GetEquippingWeapon()->GetEquipSound(), GetEquippingWeapon()->GetActorLocation());
			}
		}
		// Equip Montage와 관계없이 무기 보이게
		if (GetEquippingWeapon())
		{
			GetEquippingWeapon()->SetWeaponVisibility(true);
		}
		break;
	}
}

void UCombatComponent::OnRep_ServerCombatStateChangedState()
{
	ClearAcknowledgedCombatStateChanges(ServerCombatStateChangedState.LastCombatStateChange);

	// Unacknowledged Combat State Change 다시 적용
	if (UnacknowledgedCombatStateChanges.IsValidIndex(UnacknowledgedCombatStateChanges.Num() - 1))
	{
		FCombatStateChange CombatStateChange = UnacknowledgedCombatStateChanges[UnacknowledgedCombatStateChanges.Num() - 1];
		CombatState = CombatStateChange.CombatStateToChange;
	}

	if (IsValidOwnerCharacter() && OwnerCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	{
		ProcessChangeCombatState(ServerCombatStateChangedState.LastCombatStateChange.CombatStateToChange, ServerCombatStateChangedState.LastCombatStateChange.bPlayEquipMontage, ServerCombatStateChangedState.LastCombatStateChange.bShouldPlayUnarmedEquipMontage);
	}
}

FCombatStateChange UCombatComponent::CreateCombatStateChange(ECombatState InCombatStateToChange, bool bInPlayEquipMontage, bool bInShouldPlayUnarmedEquipMontage)
{
	if (AGameStateBase* GameStateBase = GetWorld()->GetGameState())
	{
		FCombatStateChange CombatStateChange;
		CombatStateChange.CombatStateToChange = InCombatStateToChange;
		CombatStateChange.bPlayEquipMontage = bInPlayEquipMontage;
		CombatStateChange.bShouldPlayUnarmedEquipMontage = bInShouldPlayUnarmedEquipMontage;
		CombatStateChange.Time = GameStateBase->GetServerWorldTimeSeconds();
		return CombatStateChange;
	}
	return FCombatStateChange();
}

void UCombatComponent::ServerSendCombatStateChange_Implementation(const FCombatStateChange& InCombatStateChange)
{
	ProcessChangeCombatState(InCombatStateChange.CombatStateToChange, InCombatStateChange.bPlayEquipMontage, InCombatStateChange.bShouldPlayUnarmedEquipMontage);

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
	// Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Viewport 정중앙의 크로스헤어 위치 계산 (Viewport space = screen space)
	const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

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
		SetHUDCrosshairColor(TraceHitResult.GetActor());
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
	return GetEquippingWeapon() != nullptr && bCanFire && bIsFiring && CombatState == ECombatState::ECS_Unoccupied && GetEquippingWeapon()->IsAmmoEmpty() && CarriedAmmoMap.Contains(GetEquippingWeapon()->GetWeaponType()) && CarriedAmmoMap[GetEquippingWeapon()->GetWeaponType()] == 0;
}

void UCombatComponent::DryFire()
{
	if (USoundBase* DryFireSound = GetEquippingWeapon()->GetDryFireSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, GetEquippingWeapon()->GetActorLocation());
	}
	StartDryFireTimer();
}

bool UCombatComponent::CanReloadOnFire()
{
	return GetEquippingWeapon() != nullptr && bCanFire && bIsFiring && CombatState == ECombatState::ECS_Unoccupied && GetEquippingWeapon()->IsAmmoEmpty() && CarriedAmmoMap.Contains(GetEquippingWeapon()->GetWeaponType()) && CarriedAmmoMap[GetEquippingWeapon()->GetWeaponType()] > 0;
}

void UCombatComponent::SetHUDCrosshair(EWeaponType InWeaponType)
{
	if (IsValidHUD())
	{
		HUD->DrawCrosshair(GetCrosshairTexture(InWeaponType));
	}
}

void UCombatComponent::UpdateHUDCrosshair(float DeltaTime)
{
	if (!IsValidOwnerCharacter() || !IsValidOwnerController() || !IsValidHUD())
	{
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

		CrosshairSpread = 0.5f + CrosshairSpreadVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
	}
	HUD->UpdateCrosshair(CrosshairSpread, CrosshairColor);
}

void UCombatComponent::SetHUDCrosshairColor(AActor* TracedActor)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}
	
	if (ALBlasterCharacter* TracedCharacter = Cast<ALBlasterCharacter>(TracedActor))
	{
		// 크로스헤어 하단의 이름을 표시하는 UI 업데이트
		if (ALBlasterPlayerState* TracedPlayerState = TracedCharacter->GetPlayerState<ALBlasterPlayerState>())
		{
			// 플레이어 이름 위젯 설정
            SetHUDPlayerNameTextUnderCrosshair(TracedPlayerState->GetPlayerName());

			// 크로스헤어, 플레이어 이름 위젯 색상 설정
			CrosshairColor = FLinearColor::Red;

			// 만약 같은 팀원이라면 하얀색으로 표시
			if (ALBlasterPlayerState* PlayerState = OwnerCharacter->GetPlayerState<ALBlasterPlayerState>())
			{
				if (TracedPlayerState->GetTeam() != ETeam::ET_MAX && PlayerState->GetTeam() == TracedPlayerState->GetTeam())
				{
					CrosshairColor = FLinearColor::White;
				}	
			}
		}
	}
	else
	{
		CrosshairColor = FLinearColor::White;
		SetHUDPlayerNameTextUnderCrosshair();
	}
}

void UCombatComponent::SetHUDPlayerNameTextUnderCrosshair(const FString& InPlayerName)
{
	if (IsValidHUD())
	{
		HUD->SetPlayerNameText(InPlayerName);
	}
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
	bCanFire = true;
	if (bIsFiring && bCanFire && GetEquippingWeapon() && GetEquippingWeapon()->IsAutomatic())
	{
		Fire();
	}
	else if (GetEquippingWeapon() && GetEquippingWeapon()->IsAmmoEmpty() && CarriedAmmoMap.Contains(GetEquippingWeapon()->GetWeaponType()) && CarriedAmmoMap[GetEquippingWeapon()->GetWeaponType()] > 0 && CombatState == ECombatState::ECS_Unoccupied)
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

void UCombatComponent::UpdateHUDGrenadeAmount()
{
	if (IsValidOwnerController())
	{
		OwnerController->UpdateHUDGrenadeAmount(GrenadeAmount);
	}
}

void UCombatComponent::PickupAmmo(EWeaponType InWeaponType, int32 InAmmoAmount)
{
	AddCarriedAmmo(InWeaponType, InAmmoAmount);
}

void UCombatComponent::ChooseWeaponSlot(EEquipSlot InEquipSlotType)
{
	if (EquipSlotType == InEquipSlotType || !bCanEquipWeapon)
	{
		return;
	}

	// 바꿀 슬롯에 있는 무기 장착
	if (GetEquippingWeapon(InEquipSlotType))
	{
		EquipWeapon(InEquipSlotType, EEquipMode::EEM_ChooseWeaponSlot, GetEquippingWeapon(InEquipSlotType));
	}
	else
	{
		EquipWeapon(InEquipSlotType, EEquipMode::EEM_UnarmedState);
	}
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValidOwnerCharacter())
	{
		return;
	}

	// 리스폰된 Autonomous Proxy 캐릭터 HUD 업데이트 및 DefaultWeapon 장착
	if (!OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled())
	{
		OwnerCharacter->UpdateHUDHealth();
		UpdateHUDGrenadeAmount();
		ServerEquipDefaultWeapon();
	}
}

void UCombatComponent::OnRep_GrenadeAmount()
{
	UpdateHUDGrenadeAmount();
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& LaunchLocation, const FVector_NetQuantize& LaunchVelocity)
{
	// Update GrenadeAmount
	if (OwnerCharacter->HasAuthority())
	{
		GrenadeAmount = FMath::Clamp(GrenadeAmount - 1, 0, MaxGrenadeAmount);
		UpdateHUDGrenadeAmount();
	}
	
	MulticastLaunchGrenade(LaunchLocation, LaunchVelocity);
}

void UCombatComponent::MulticastLaunchGrenade_Implementation(const FVector_NetQuantize& LaunchLocation, const FVector_NetQuantize& LaunchVelocity)
{
	// 중복 Launch 방지
	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled())
	{
		return;
	}
	LocalLaunchGrenade(LaunchLocation, LaunchVelocity);
}

void UCombatComponent::LocalLaunchGrenade(const FVector_NetQuantize& LaunchLocation, const FVector_NetQuantize& LaunchVelocity)
{
	if (IsValidOwnerCharacter() && OwnerCharacter->GetAttachedGrenade() && GrenadeClass && GetWorld())
	{
		ShowAttachedGrenade(false);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = SpawnParams.Instigator = OwnerCharacter;
		if (AThrowableGrenade* Grenade = GetWorld()->SpawnActor<AThrowableGrenade>(GrenadeClass, LaunchLocation, LaunchVelocity.Rotation(), SpawnParams))
		{
			Grenade->SetInitialVelocity(LaunchVelocity);

			if (OwnerCharacter->HasAuthority())
			{
				Grenade->SetReplicatesPostInit(false);
			}
		}
	}
}

void UCombatComponent::HandleUnEquipBeforeTossGrenade()
{
	if (IsValidOwnerCharacter() && UnEquipBeforeTossGrenadeMontage)
	{
		OwnerCharacter->PlayTossGrenadeMontage(UnEquipBeforeTossGrenadeMontage, false);
	}
}

void UCombatComponent::ShowAttachedGrenade(bool bShow)
{
	if (IsValidOwnerCharacter() && OwnerCharacter->GetAttachedGrenade())
	{
		OwnerCharacter->GetAttachedGrenade()->SetVisibility(bShow);
	}
}

void UCombatComponent::HideAllSplineMesh()
{
	// Spine에 포함되지 않는 SplineMesh는 숨김
	for (int32 Index = 0; Index < SplineMeshes.Num(); ++Index)
	{
		if (SplineMeshes[Index])
		{
			SplineMeshes[Index]->SetVisibility(false);
		}
	}
}

void UCombatComponent::EquipFinished()
{
	ChangeCombatState(ECombatState::ECS_Unoccupied);

	// Aiming이 Interrupt 됐을 때 다시 Aiming
	if (OwnerCharacter->IsLocallyControlled() && bAimingKeyPressed && !bIsAiming && GetEquippingWeapon())
	{
		SetAiming(true);
	}
	
	if (bIsFiring && bCanFire && GetEquippingWeapon() && GetEquippingWeapon()->IsAutomatic())
	{
		Fire();
	}
}

void UCombatComponent::HideCrosshair()
{
	// Tick에서의 추가 연산 방지
	bShowCrosshair = false;

	// Unarmed State에서는 크로스헤어를 그리지 않음.
	SetHUDCrosshair(EWeaponType::EWT_Unarmed);
}

void UCombatComponent::ShowCrosshair(EWeaponType InWeaponType)
{
	bShowCrosshair = true;
	SetHUDCrosshair(InWeaponType);	
}

void UCombatComponent::DrawGrenadeTrajectory()
{
	if (IsValidOwnerCharacter() && OwnerCharacter->GetAttachedGrenade() && GetWorld())
	{
		const FVector StartingLocation = OwnerCharacter->GetAttachedGrenade()->GetComponentLocation();
		GrenadeLaunchLocation = StartingLocation;
		
		FVector Target = TraceHitTarget;
		FVector ToTargetDir = (Target - StartingLocation).GetSafeNormal();
		if ((TraceHitTarget - StartingLocation).SizeSquared() > MaxGrenadeThrowDistance * MaxGrenadeThrowDistance)
		{
			Target = StartingLocation + ToTargetDir * MaxGrenadeThrowDistance;
			ToTargetDir = (Target - StartingLocation).GetSafeNormal();
		}
		
		// 발사 Velocity 설정
		FVector OutVelocity;
		if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVelocity, StartingLocation, Target, 0.f, GrenadePathArcValue))
		{
			OutVelocity = OutVelocity.GetSafeNormal() * 1500.f;
			GrenadeLaunchVelocity = OutVelocity;
			
			if (USplineComponent* GrenadeSplineComponent = OwnerCharacter->GetGrenadeSplineComponent())
			{
				GrenadeSplineComponent->ClearSplinePoints();

				if (!bDrawGrenadeTrajectory)
				{
					HideAllSplineMesh();
					return;
				}
				
				// 수류탄 궤적
				FPredictProjectilePathParams PredictParams(5.f, StartingLocation, OutVelocity, 15.0f);
				PredictParams.ActorsToIgnore.Add(OwnerCharacter);
				//PredictParams.DrawDebugTime = 15.0f;     //디버그 라인 보여지는 시간 (초)
				//PredictParams.DrawDebugType = EDrawDebugTrace::Type::ForOneFrame;  // DrawDebugTime 을 지정하면 EDrawDebugTrace::Type::ForDuration 필요.
				FPredictProjectilePathResult Result;
				UGameplayStatics::PredictProjectilePath(this, PredictParams, Result);
				const TArray<FPredictProjectilePathPointData>& PathData = Result.PathData;
				
				for (int32 Index = 0; Index < PathData.Num(); ++Index)
				{
					GrenadeSplineComponent->AddSplinePointAtIndex(PathData[Index].Location, Index, ESplineCoordinateSpace::Local);
				}

				// Spline Mesh Component 그리기
				int32 Index = 0;
				for (; Index < GrenadeSplineComponent->GetNumberOfSplinePoints(); ++Index)
				{
					// 해당 인덱스에 이미 생성된 USplineMeshComponent가 없으면 새로 생성
					if (!SplineMeshes.IsValidIndex(Index) && GrenadeTrajectorySM)
					{
						if (USplineMeshComponent* NewSplineMeshComp = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass()))
						{
							NewSplineMeshComp->SetStaticMesh(GrenadeTrajectorySM);
							NewSplineMeshComp->SetMobility(EComponentMobility::Movable);
							NewSplineMeshComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
							NewSplineMeshComp->RegisterComponentWithWorld(GetWorld());
							NewSplineMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
							NewSplineMeshComp->SetCastShadow(false);
							SplineMeshes.Add(NewSplineMeshComp);
						}
					}
					if (SplineMeshes[Index])
					{
						FVector StartLocation, StartTangent, EndLocation, EndTangent;
						GrenadeSplineComponent->GetLocationAndTangentAtSplinePoint(Index, StartLocation, StartTangent, ESplineCoordinateSpace::Local);
						GrenadeSplineComponent->GetLocationAndTangentAtSplinePoint(Index + 1, EndLocation, EndTangent, ESplineCoordinateSpace::Local);
						SplineMeshes[Index]->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
						SplineMeshes[Index]->SetVisibility(true);
					}
				}
				// Spine에 포함되지 않는 SplineMesh는 숨김
				for (; Index < SplineMeshes.Num(); ++Index)
				{
					if (SplineMeshes[Index])
					{
						SplineMeshes[Index]->SetVisibility(false);
					}
				}
			}
		}
	}
}

FString UCombatComponent::GetCombatInfo()
{
	FString Str(TEXT("CombatInfo : "));
	Str += FString::Printf(TEXT("[EquipSlotType %s] "), LexToString(EquipSlotType));
	Str += FString::Printf(TEXT("[Valid EquippingWeapon %s] "), *LexToString(GetEquippingWeapon() != nullptr));
	if (GetEquippingWeapon())
	{
		Str += FString::Printf(TEXT("[EquippingWeapon Has Ammo %s] "), *LexToString(!GetEquippingWeapon()->IsAmmoEmpty()));
	}
	Str += FString::Printf(TEXT("[bCanFire %s] "), *LexToString(bCanFire));
	Str += FString::Printf(TEXT("[bIsFiring %s] "), *LexToString(static_cast<bool>(bIsFiring)));
	Str += FString::Printf(TEXT("[CombatState %s] "), LexToString(CombatState));

	return Str;
}

FCrosshairTexture UCombatComponent::GetCrosshairTexture(EWeaponType InWeaponType) const
{
	if (InWeaponType == EWeaponType::EWT_Unarmed)
	{
		return FCrosshairTexture();
	}
	if (InWeaponType == EWeaponType::EWT_Shotgun)
	{
		return ShotgunCrosshair;
	}
	return DefaultCrosshair;
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

void UCombatComponent::TossGrenade(bool bPressed)
{
	if (GrenadeAmount <= 0)
	{
		return;
	}
	
	if (bPressed)
	{
		if (CombatState == ECombatState::ECS_Unoccupied)
		{
			bCanLaunchGrenade = true;
			// Local & Server Call HandleUnEquipBeforeTossGrenade()
			ChangeCombatState(ECombatState::ECS_TossingGrenade);
		}
	}
	else
	{
		if (bCanLaunchGrenade && CombatState == ECombatState::ECS_TossingGrenade)
		{
			bCanLaunchGrenade = false;
			StartTossGrenade(true);
		}
	}
}

void UCombatComponent::TossGrenadeFinished()
{
	if (GetEquippingWeapon())
	{
		ChangeCombatState(ECombatState::ECS_Equipping);
	}
	else
	{
		ChangeCombatState(ECombatState::ECS_Unoccupied);
	}
}

void UCombatComponent::StartTossGrenade(bool bShouldJumpToSection)
{
	if (IsValidOwnerCharacter() && TossGrenadeMontage)
	{
		if (bShouldJumpToSection)
		{
			bDrawGrenadeTrajectory = false;
		}
		OwnerCharacter->PlayTossGrenadeMontage(TossGrenadeMontage, bShouldJumpToSection);
		ShowAttachedGrenade(true);
	}
}

void UCombatComponent::LaunchGrenade()
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}

	// Update GrenadeAmount
	if (OwnerCharacter->HasAuthority())
	{
		GrenadeAmount = FMath::Clamp(GrenadeAmount - 1, 0, MaxGrenadeAmount);
		UpdateHUDGrenadeAmount();
	}

	if (OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		LocalLaunchGrenade(GrenadeLaunchLocation, GrenadeLaunchVelocity);
		ServerLaunchGrenade(GrenadeLaunchLocation, GrenadeLaunchVelocity);
	}
	// 서버 Local Character
	if (OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled())
	{
		LocalLaunchGrenade(GrenadeLaunchLocation, GrenadeLaunchVelocity);
		MulticastLaunchGrenade(GrenadeLaunchLocation, GrenadeLaunchVelocity);
	}
}

void UCombatComponent::SetFiring(bool bInFiring)
{
	if (!IsValidOwnerCharacter() || !GetEquippingWeapon())
	{
		return;
	}

	bIsFiring = bInFiring;
	if (bIsFiring)
	{
		// 무기 장착, 재장전 중에 발사 키를 입력 중이면 총을 돌리지 않고 바로 조준하고 발사하도록
		if (GetEquippingWeapon()->IsAutomatic() && (CombatState == ECombatState::ECS_Equipping || CombatState == ECombatState::ECS_Reloading))
		{
			bCanAnimateFiring = true;
		}
		Fire();
	}
	else
	{
		bCanAnimateFiring = false;
	}

	// Client Side Prediction for bIsFiring
	if (OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		bDesiredIsFiring = bIsFiring;
		ServerSetFiring(bIsFiring, bCanAnimateFiring);
	}
}

void UCombatComponent::ServerSetFiring_Implementation(bool bInFiring, bool bInCanAnimateFiring)
{
	bIsFiring = bInFiring;
	bCanAnimateFiring = bInCanAnimateFiring;
}

void UCombatComponent::OnRep_IsFiring()
{
	if (IsValidOwnerCharacter())
	{
		// Client Side Prediction for bIsFiring
		if (OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
		{
			bIsFiring = bDesiredIsFiring;
		}
		// for Simulated Proxy Character Firing Animation
		else if (OwnerCharacter->GetLocalRole() == ROLE_SimulatedProxy)
		{
			if (bIsFiring)
			{
				// 무기 장착, 재장전 중에 발사 키를 입력 중이면 총을 돌리지 않고 바로 조준하고 발사하도록
				if (GetEquippingWeapon()->IsAutomatic() && (CombatState == ECombatState::ECS_Equipping || CombatState == ECombatState::ECS_Reloading))
				{
					bCanAnimateFiring = true;
				}
			}
			else
			{
				bCanAnimateFiring = false;
			}	
		}
	}
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

		// 로컬에서의 총구 Location, Rotation 캐싱
		FVector_NetQuantize MuzzleFlashLocation;
		FRotator MuzzleFlashRotation;
		if (!GetEquippingWeapon()->GetMuzzleFlashLocation(MuzzleFlashLocation, MuzzleFlashRotation))
		{
			return;
		}

		if (GetEquippingWeapon()->GetWeaponType() == EWeaponType::EWT_Shotgun)
		{
			// Shotgun은 Scatter를 적용한 Pellet 개수만큼의 TraceHitTargets를 사용
			const TArray<FVector_NetQuantize>& TraceHitTargets = GetEquippingWeapon()->ShotgunTraceEndWithScatter(MuzzleFlashLocation, TraceHitTarget);

			ShotgunLocalFire(MuzzleFlashLocation, MuzzleFlashRotation, TraceHitTargets);
			ServerShotgunFire(MuzzleFlashLocation, MuzzleFlashRotation, TraceHitTargets, OwnerCharacter->IsServerSideRewindEnabled());
		}
		else
		{
			// Scatter가 적용된 TraceHitTarget로 업데이트
			if (GetEquippingWeapon()->DoesUseScatter())
			{
				TraceHitTarget = GetEquippingWeapon()->TraceEndWithScatter(MuzzleFlashLocation, TraceHitTarget);
			}
		
			// Fire Montage등 cosmetic effect는 로컬에서 먼저 수행
			LocalFire(MuzzleFlashLocation, MuzzleFlashRotation, TraceHitTarget);	
			ServerFire(MuzzleFlashLocation, MuzzleFlashRotation, TraceHitTarget, OwnerCharacter->IsServerSideRewindEnabled());
		}
		
		CrosshairShootingFactor = 0.75f;
		StartFireTimer();

		// 수직 반동
		OwnerCharacter->AddControllerPitchInput(GetEquippingWeapon()->GetVerticalRecoilValue());
	}
	else if (CanReloadOnFire())
	{
		Reload();
	}
	else if (CanDryFire())
	{
		DryFire();
	}
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector_NetQuantize& HitTarget)
{
	if (IsValidOwnerCharacter() && GetEquippingWeapon())
	{
		if (UAnimMontage* MontageToPlay = FireMontages[GetEquippingWeapon()->GetWeaponType()])
		{
			OwnerCharacter->PlayFireMontage(MontageToPlay);
		}
		GetEquippingWeapon()->Fire(TraceStart, TraceRotation, HitTarget);

		// 총을 실제로 발사하면 Firing Animation 재생하도록
		bCanAnimateFiring = true;
	}
}

void UCombatComponent::ShotgunLocalFire(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const TArray<FVector_NetQuantize>& HitTargets)
{
	if (IsValidOwnerCharacter() && GetEquippingWeapon())
	{
		if (UAnimMontage* MontageToPlay = FireMontages[GetEquippingWeapon()->GetWeaponType()])
		{
			OwnerCharacter->PlayFireMontage(MontageToPlay);
		}
		GetEquippingWeapon()->ShotgunFire(TraceStart, TraceRotation, HitTargets);
		
		// 총을 실제로 발사하면 Firing Animation 재생하도록
		bCanAnimateFiring = true;
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector_NetQuantize& HitTarget, bool bEnabledSSR)
{
	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->EnableServerSideRewind(bEnabledSSR);
	}
	MulticastFire(TraceStart, TraceRotation, HitTarget);	
}

void UCombatComponent::ServerShotgunFire_Implementation(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const TArray<FVector_NetQuantize>& HitTargets, bool bEnabledSSR)
{
	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->EnableServerSideRewind(bEnabledSSR);
	}
	MulticastShotgunFire(TraceStart, TraceRotation, HitTargets);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const FVector_NetQuantize& HitTarget)
{
	// Locally Controlled Character의 Local Fire 중복 호출 방지
	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	LocalFire(TraceStart, TraceRotation, HitTarget);
}

void UCombatComponent::MulticastShotgunFire_Implementation(const FVector_NetQuantize& TraceStart, const FRotator& TraceRotation, const TArray<FVector_NetQuantize>& HitTargets)
{
	// Locally Controlled Character의 Local Fire 중복 호출 방지
	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	ShotgunLocalFire(TraceStart, TraceRotation, HitTargets);
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
		if (AWeapon* SpawnedDefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass))
		{
			DefaultWeapon = SpawnedDefaultWeapon; 
			ProcessEquipWeapon(EquipSlotType, EEquipMode::EEM_OverlappingWeapon, DefaultWeapon, true);
		}
	}
}

void UCombatComponent::OnRep_DefaultWeapon()
{
	if (!bEquipDefaultWeapon)
	{
		bEquipDefaultWeapon = true;
		ProcessEquipWeapon(EEquipSlot::EES_ThirdSlot, EEquipMode::EEM_OverlappingWeapon, DefaultWeapon, true);
	}
}

void UCombatComponent::OnEquipDelayTimerFinished()
{
	bCanEquipWeapon = true;
	EquipNewOverlappingWeapon();
}

AWeapon* UCombatComponent::GetEquippingWeapon(EEquipSlot InEquipSlotType)
{
	return EquipSlots[static_cast<uint8>(InEquipSlotType)];
}

FTransform UCombatComponent::GetWeaponLeftHandTransform()
{
	if (GetEquippingWeapon())
	{
		return GetEquippingWeapon()->GetWeaponMesh()->GetSocketTransform(FName(TEXT("LeftHandSocket")), RTS_ParentBoneSpace);
	}
	return FTransform::Identity;
}

void UCombatComponent::EquipOverlappingWeapon(AWeapon* InWeapon)
{
	if (!bCanEquipWeapon)
	{
		return;
	}

	if (IsValidOwnerCharacter() && InWeapon)
	{
		EquipWeapon(EquipSlotType, EEquipMode::EEM_OverlappingWeapon, InWeapon);
	}
}

void UCombatComponent::EquipNewOverlappingWeapon()
{
	// DefaultWeapon 장착 주에는 새로 장착 X
	if (EquipSlotType == EEquipSlot::EES_ThirdSlot)
	{
		return;
	}
	
	if (IsValidOwnerCharacter() && OwnerCharacter->IsLocallyControlled())
	{
		TArray<AActor*> OverlappingActors;
		OwnerCharacter->GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, AWeapon::StaticClass());
		if (OverlappingActors.Num() == 0)
		{
			return;
		}

		// 캐릭터와 가장 가까운 Overlapping Weapon을 장착
		const FVector Location = OwnerCharacter->GetActorLocation();
		float MinDistance = MAX_flt;
		AActor* NearestActor = nullptr;
		for (AActor* Actor : OverlappingActors)
		{
			// 이미 장착된 무기는 착용할 수 없도록
			if (Actor->GetOwner())
			{
				continue;
			}
			
			if (const float Distance = (Location - Actor->GetActorLocation()).Length(); Distance < MinDistance)
			{
				NearestActor = Actor;
				MinDistance = Distance;
			}
		}
		if (NearestActor)
		{
			if (AWeapon* NearestWeapon = Cast<AWeapon>(NearestActor))
			{
				EquipOverlappingWeapon(NearestWeapon);
			}    
		}
	}
}

void UCombatComponent::EquipWeapon(EEquipSlot InEquipSlotType, EEquipMode InEquipMode, AWeapon* InWeaponToEquip)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}

	// 수류탄 Hold 중이면 Toss Grenade를 취소하도 EquipWeapon 수행
	if (CombatState == ECombatState::ECS_TossingGrenade)
	{
		// 수류탄 Launch 중이면 방지
		if (!bCanLaunchGrenade)
		{
			return;
		}
		bCanLaunchGrenade = true;
		bDrawGrenadeTrajectory = false;
		HideAllSplineMesh();
		TossGrenadeFinished();
		OwnerCharacter->StopAnimMontage();
	}

	FWeaponEquip WeaponEquip = CreateWeaponEquip(InEquipSlotType, InEquipMode, InWeaponToEquip, true);
	if (OwnerCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		ProcessEquipWeapon(InEquipSlotType, InEquipMode, InWeaponToEquip, true, false);
		UnacknowledgedWeaponEquips.Add(WeaponEquip);
		ServerSendWeaponEquip(WeaponEquip);
	}
	if (OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled())
	{
		ServerSendWeaponEquip(WeaponEquip);
	}
}

void UCombatComponent::ProcessEquipWeapon(EEquipSlot InEquipSlotType, EEquipMode InEquipMode, AWeapon* InWeaponToEquip, bool bPlayEquipMontage, bool bCanSendCombatStateRPC)
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}

	if (OwnerCharacter->IsLocallyControlled())
	{
		// Set Equip Delay Timer
		bCanEquipWeapon = false;
		GetWorld()->GetTimerManager().SetTimer(EquipDelayTimer, this, &ThisClass::OnEquipDelayTimerFinished, EquipDelay, false);

		// Interrupt Aiming for UnarmedState
		SetAiming(false);
	}
	
	EEquipSlot PrevSlotType = EquipSlotType;
	EquipSlotType = InEquipSlotType;

	// Switch to Unarmed State
	if (InEquipMode == EEquipMode::EEM_UnarmedState)
	{
		// 슬롯 바꾸기 전에 들고 있던 무기 손에서 떼야함.
		HolsterWeapon(PrevSlotType);	
		
		// 이전 슬롯과 현재 슬롯이 모두 Unarmed라면 Equip Montage 재생 X
		const bool bShouldPlayUnarmedEquipMontage = bPlayEquipMontage && EquipSlots[static_cast<uint8>(PrevSlotType)];
		if (bShouldPlayUnarmedEquipMontage)
		{
			OwnerCharacter->SetWeaponAnimLayers(EWeaponType::EWT_Unarmed);
			ChangeCombatState(ECombatState::ECS_Equipping, bPlayEquipMontage, bShouldPlayUnarmedEquipMontage, bCanSendCombatStateRPC);
		}
		else
		{
			ChangeCombatState(ECombatState::ECS_Unoccupied, bPlayEquipMontage, bShouldPlayUnarmedEquipMontage, bCanSendCombatStateRPC);
		}
		
		if (IsValidOwnerController() && OwnerCharacter->IsLocallyControlled())
		{
			OwnerController->SetHUDAmmo(0);
			OwnerController->SetHUDCarriedAmmo(0);
			OwnerController->SetHUDWeaponTypeText(GetWeaponTypeString());
			OwnerController->SetWeaponSlotIcon(EquipSlotType, EWeaponType::EWT_Unarmed);
			OwnerController->ChooseWeaponSlot(EquipSlotType);
			HideCrosshair();
		}

		// Unarmed State로 변경하면 Firing Animation 정지
		bCanAnimateFiring = false;
		
		return;
	}

	// Equip Overlapping Weapon
	if (InEquipMode == EEquipMode::EEM_OverlappingWeapon)
	{
		// 기존에 착용 중인 무기는 제거
		ElimWeapon();
	}
	// Choose Weapon Slot
	else if (InEquipMode == EEquipMode::EEM_ChooseWeaponSlot)
	{
		// 슬롯 바꾸기 전에 들고 있던 무기 손에서 떼야함.
		HolsterWeapon(PrevSlotType);
	}
	
	// Restore Weapon To Equip
    if (InWeaponToEquip)
    {
    	EquipSlots[static_cast<uint8>(EquipSlotType)] = InWeaponToEquip;
    }
	if (GetEquippingWeapon())
	{
		GetEquippingWeapon()->SetOwner(OwnerCharacter);
		
		AttachWeapon();
		OwnerCharacter->SetWeaponAnimLayers(GetEquippingWeapon()->GetWeaponType(), GetEquippingWeapon()->GetWeaponAnimLayer());
		ChangeCombatState(ECombatState::ECS_Equipping, bPlayEquipMontage, false, bCanSendCombatStateRPC);

		GetEquippingWeapon()->OnWeaponEquipped(true); 
		GetEquippingWeapon()->SetHUDAmmo();
		
		if (IsValidOwnerController() && OwnerCharacter->IsLocallyControlled())
		{
			if (CarriedAmmoMap.Contains(GetEquippingWeapon()->GetWeaponType()))
			{
				OwnerController->SetHUDCarriedAmmo(CarriedAmmoMap[GetEquippingWeapon()->GetWeaponType()]);
				OwnerController->SetHUDWeaponTypeText(GetWeaponTypeString(GetEquippingWeapon()->GetWeaponType()));
				OwnerController->SetWeaponSlotIcon(EquipSlotType, GetEquippingWeapon()->GetWeaponType());
				OwnerController->ChooseWeaponSlot(EquipSlotType);
			}
			ShowCrosshair(GetEquippingWeapon()->GetWeaponType());
		}

		/* ADS FOV */
		OwnerCharacter->SetAdsFov(GetEquippingWeapon()->GetADSFOV());
			
		/* Sniper Scope */
		InitSniperScope();
	}
}

void UCombatComponent::OnRep_ServerWeaponEquipState()
{
	ClearAcknowledgedWeaponEquip(ServerWeaponEquipState.LastWeaponEquip);
	
	// Unacknowledged WeaponEquip 다시 적용
	if (UnacknowledgedWeaponEquips.IsValidIndex(UnacknowledgedWeaponEquips.Num() - 1))
	{
		const FWeaponEquip WeaponEquip = UnacknowledgedWeaponEquips[UnacknowledgedWeaponEquips.Num() - 1];
		
		// 착용 중인 무기와 다른 경우만 착용
		if (GetEquippingWeapon() != WeaponEquip.WeaponToEquip)
		{
			ProcessEquipWeapon(WeaponEquip.SlotToEquip, WeaponEquip.EquipMode, WeaponEquip.WeaponToEquip, false, false);
		}
	}

	if (IsValidOwnerCharacter() && OwnerCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	{
		ProcessEquipWeapon(ServerWeaponEquipState.LastWeaponEquip.SlotToEquip, ServerWeaponEquipState.LastWeaponEquip.EquipMode, ServerWeaponEquipState.LastWeaponEquip.WeaponToEquip, ServerWeaponEquipState.LastWeaponEquip.bPlayEquipMontage, false);
	}
}

FWeaponEquip UCombatComponent::CreateWeaponEquip(EEquipSlot InEquipSlotType, EEquipMode InEquipMode, AWeapon* InWeaponToEquip, bool bInPlayEquipMontage)
{
	if (AGameStateBase* GameStateBase = GetWorld()->GetGameState())
	{
		FWeaponEquip WeaponEquip;
		WeaponEquip.SlotToEquip = InEquipSlotType;
		WeaponEquip.WeaponToEquip = InWeaponToEquip;
		WeaponEquip.EquipMode = InEquipMode;
		WeaponEquip.bPlayEquipMontage = bInPlayEquipMontage;
		WeaponEquip.Time = GameStateBase->GetServerWorldTimeSeconds();
		return WeaponEquip;
	}
	return FWeaponEquip();
}

void UCombatComponent::ServerSendWeaponEquip_Implementation(const FWeaponEquip& InWeaponEquip)
{
	ProcessEquipWeapon(InWeaponEquip.SlotToEquip, InWeaponEquip.EquipMode, InWeaponEquip.WeaponToEquip, InWeaponEquip.bPlayEquipMontage);

	ServerWeaponEquipState.EquippingSlot = EquipSlotType;
	ServerWeaponEquipState.EquippingWeapon = GetEquippingWeapon();
	ServerWeaponEquipState.EquipMode = InWeaponEquip.EquipMode;
	ServerWeaponEquipState.LastWeaponEquip = InWeaponEquip;
}

void UCombatComponent::ClearAcknowledgedWeaponEquip(const FWeaponEquip& LastWeaponEquip)
{
	TArray<FWeaponEquip> NewArray;

	for (const FWeaponEquip& WeaponEquip : UnacknowledgedWeaponEquips)
	{
		if (WeaponEquip.Time > LastWeaponEquip.Time)
		{
			NewArray.Add(WeaponEquip);
		}
	}
	UnacknowledgedWeaponEquips = NewArray;
}

void UCombatComponent::HolsterWeapon(EEquipSlot InEquipSlotType)
{
	if (GetEquippingWeapon(InEquipSlotType))
	{
		GetEquippingWeapon(InEquipSlotType)->Holstered();
	}
}
