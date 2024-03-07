// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Character/LBlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/LBlasterHUD.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/LBlasterPlayerController.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	/* Aiming */
	ADSMultiplier = 0.5f;

	/* Ammo */
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Rifle, 0);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, 0);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, 0);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, 0);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, 0);

	/* Fire */
	FireMontages.Emplace(EWeaponType::EWT_Rifle, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_RocketLauncher, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_Pistol, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_SMG, nullptr);
	FireMontages.Emplace(EWeaponType::EWT_Shotgun, nullptr);

	/* Reload */
	ReloadMontages.Emplace(EWeaponType::EWT_Rifle, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_RocketLauncher, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_Pistol, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_SMG, nullptr);
	ReloadMontages.Emplace(EWeaponType::EWT_Shotgun, nullptr);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippingWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
	DOREPLIFETIME(UCombatComponent, bIsFiring);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Tick에서 Trace를 통해 크로스헤어 색상 설정
	FHitResult HitResult;
	TraceUnderCrosshair(HitResult);
	// 총구 정렬
	SetRightHandRotation(HitResult.ImpactPoint);
	
	// 크로스헤어 Draw
	SetHUDCrosshair(DeltaTime);
}

void UCombatComponent::SetAiming(bool bInAiming)
{
	if (!EquippingWeapon)
	{
		return;
	}
	
	bIsAiming = bInAiming;
	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->SetADSWalkSpeed(bInAiming, ADSMultiplier);
		OwnerCharacter->SetBlendWeight(0.f);
	}
	
	ServerSetAiming(bInAiming);
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
	if (!EquippingWeapon)
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
	if (!EquippingWeapon || !ReloadMontages.Contains(EquippingWeapon->GetWeaponType()))
	{
		return nullptr;
	}
	return ReloadMontages[EquippingWeapon->GetWeaponType()];
}

void UCombatComponent::DropWeapon()
{
	if (EquippingWeapon)
	{
		EquippingWeapon->Dropped();
		EquippingWeapon = nullptr;
	}
}

void UCombatComponent::Reload()
{
	if (EquippingWeapon && EquippingWeapon->NeedReload() && CarriedAmmo > 0 && CombatState != ECombatState::ECS_Reloading)
	{
		ServerReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	CombatState = ECombatState::ECS_Reloading;
	HandleReload();
}

void UCombatComponent::HandleReload()
{
	if (IsValidOwnerCharacter())
	{
		OwnerCharacter->PlayReloadMontage(SelectReloadMontage());
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if (!IsValidOwnerCharacter() || !IsValidOwnerController() || !EquippingWeapon)
	{
		return;
	}

	const int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippingWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippingWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippingWeapon->GetWeaponType()];
	}
	OwnerController->SetHUDCarriedAmmo(CarriedAmmo);
	EquippingWeapon->AddAmmo(ReloadAmount);
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippingWeapon)
	{
		int32 RoomInMag = EquippingWeapon->GetRoomInMag();
		if (CarriedAmmoMap.Contains(EquippingWeapon->GetWeaponType()))
		{
			return FMath::Min(CarriedAmmoMap[EquippingWeapon->GetWeaponType()], RoomInMag);
		}
	}
	return 0;
}

void UCombatComponent::MulticastInterruptReload_Implementation()
{
	if (CombatState == ECombatState::ECS_Reloading)
	{
		if (IsValidOwnerCharacter())
		{
			if (OwnerCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
			{
				OwnerCharacter->GetMesh()->GetAnimInstance()->StopAllMontages(0.1f);
			}
		}
		
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::ReloadFinished()
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}

	CombatState = ECombatState::ECS_Unoccupied;
	if (OwnerCharacter->HasAuthority())
	{
		UpdateAmmoValues();
	}

	if (OwnerCharacter->IsLocallyControlled() && bIsFiring)
	{
		Fire();
	}
}

bool UCombatComponent::IsValidOwnerCharacter()
{
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(GetOwner());
	}
	return OwnerCharacter != nullptr;
}

bool UCombatComponent::IsValidOwnerController()
{
	if (!OwnerController)
	{
		OwnerController = Cast<ALBlasterPlayerController>(OwnerCharacter->Controller);
	}
	return OwnerController != nullptr;
}

bool UCombatComponent::IsValidHUD()
{
	if (!HUD)
	{
		HUD = Cast<ALBlasterHUD>(OwnerController->GetHUD());
	}
	return HUD != nullptr;
}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	if (!EquippingWeapon)
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
	
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	if (bScreenToWorld)
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

void UCombatComponent::SetHUDCrosshair(float DeltaTime)
{
	if (!IsValidOwnerCharacter() || !IsValidOwnerController() || !IsValidHUD())
	{
		return;
	}

	if (EquippingWeapon)
	{
		HUDPackage.TopCrosshair = EquippingWeapon->TopCrosshair;
		HUDPackage.BottomCrosshair = EquippingWeapon->BottomCrosshair;
		HUDPackage.LeftCrosshair = EquippingWeapon->LeftCrosshair;
		HUDPackage.RightCrosshair = EquippingWeapon->RightCrosshair;
		HUDPackage.CenterCrosshair = EquippingWeapon->CenterCrosshair;
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
	if (!IsValidOwnerCharacter() || !EquippingWeapon)
	{
		return;
	}

	OwnerCharacter->GetWorldTimerManager().SetTimer(FireTimer, this, &ThisClass::FireTimerFinished, EquippingWeapon->GetFireDelay());
}

void UCombatComponent::FireTimerFinished()
{
	if (!EquippingWeapon)
	{
		return;
	}

	bCanFire = true;
	if (bIsFiring && EquippingWeapon->IsAutomatic())
	{
		Fire();
	}

	if (EquippingWeapon->IsAmmoEmpty())
	{
		Reload();
	}
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	if (IsValidOwnerController())
	{
		OwnerController->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::OnRep_CombatState()
{
	// TODO: CombatState 모든 경우 처리
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();	
		break;
	}
}

bool UCombatComponent::CanFire() const
{
	return EquippingWeapon != nullptr && !EquippingWeapon->IsAmmoEmpty() && bCanFire && bIsFiring && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		ServerFire(HitResult.ImpactPoint);
		CrosshairShootingFactor = 0.75f;
		StartFireTimer();
		
		// Debug Line
		// FTransform MuzzleTipTransform = EquippingWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), RTS_World);
		// FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		// DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red, false, 20.f, 0, 2.f);
		// DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), HitResult.ImpactPoint, FColor::Orange, false, 20.f, 0, 2.f);
	}
}

void UCombatComponent::ServerSetFiring_Implementation(bool bInFiring)
{
	bIsFiring = bInFiring;
}

void UCombatComponent::OnRep_EquippingWeapon()
{
	if (EquippingWeapon)
	{
		EquippingWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		
		if (IsValidOwnerCharacter())
		{
			if (IsValidOwnerController())
			{
				OwnerController->SetHUDWeaponTypeText(GetWeaponTypeString(EquippingWeapon->GetWeaponType()));
			}
			
			OwnerCharacter->AttachWeapon(EquippingWeapon);
			OwnerCharacter->SetWeaponAnimLayers(EquippingWeapon->GetWeaponAnimLayer());
			UGameplayStatics::PlaySoundAtLocation(this, EquippingWeapon->GetEquipSound(), EquippingWeapon->GetActorLocation());
		}	
	}
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
	return FString();
}

void UCombatComponent::SetRightHandRotation(const FVector& HitTarget)
{
	if (IsValidOwnerCharacter())
	{
		const FTransform RightHandTransform = OwnerCharacter->GetMesh()->GetSocketTransform(FName(TEXT("hand_r")), RTS_World);
		RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - HitTarget));

		// TODO : RightHandRotation Clamp
		// const FTransform RightHandSocketTransform = OwnerCharacter->GetMesh()->GetSocketTransform(FName(TEXT("RightHandSocket")), RTS_World);
		// UE_LOG(LogTemp, Warning, TEXT("RHSTR %s"), *RightHandSocketTransform.Rotator().ToString());
		// const FRotator BaseRot(0.f, RightHandSocketTransform.Rotator().Yaw - 90.f, 0.f);
		// UE_LOG(LogTemp, Warning, TEXT("BaseRot %s"), *BaseRot.ToString());
		// RightHandRotation.Pitch = FMath::Min(RightHandRotation.Pitch, BaseRot.Pitch + 30.f);
		// RightHandRotation.Yaw = FMath::Clamp(RightHandRotation.Yaw, BaseRot.Yaw - 20.f, BaseRot.Yaw + 20.f);
		// UE_LOG(LogTemp, Warning, TEXT("RHR %s"), *RightHandRotation.ToString());
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (IsValidOwnerCharacter())
	{
		if (UAnimMontage* MontageToPlay = FireMontages[EquippingWeapon->GetWeaponType()])
		{
			OwnerCharacter->PlayFireMontage(MontageToPlay);
		}
		if (EquippingWeapon)
		{
			EquippingWeapon->Fire(TraceHitTarget);
		}
	}
}

void UCombatComponent::EquipWeapon(AWeapon* InWeapon)
{
	// 이미 무기를 장착하고 있으면 기존 무기는 Drop
	if (EquippingWeapon)
	{
		MulticastInterruptReload();
		EquippingWeapon->Dropped();
	}
	
	// From ServerRPC (Server Only)
	EquippingWeapon = InWeapon;

	if (EquippingWeapon)
	{
		EquippingWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		if (IsValidOwnerCharacter())
		{
			EquippingWeapon->SetOwner(OwnerCharacter);
			
			EquippingWeapon->SetHUDAmmo();
			if (CarriedAmmoMap.Contains(EquippingWeapon->GetWeaponType()))
			{
				CarriedAmmo = CarriedAmmoMap[EquippingWeapon->GetWeaponType()];
				if (IsValidOwnerController())
				{
					OwnerController->SetHUDCarriedAmmo(CarriedAmmo);
					OwnerController->SetHUDWeaponTypeText(GetWeaponTypeString(EquippingWeapon->GetWeaponType()));
				}
			}
			
			OwnerCharacter->AttachWeapon(EquippingWeapon);
			OwnerCharacter->SetWeaponAnimLayers(EquippingWeapon->GetWeaponAnimLayer());
			UGameplayStatics::PlaySoundAtLocation(this, EquippingWeapon->GetEquipSound(), EquippingWeapon->GetActorLocation());
		}

		if (EquippingWeapon->IsAmmoEmpty())
		{
			Reload();
		}
	}
}

