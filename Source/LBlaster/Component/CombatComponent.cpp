// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"

#include "Camera/CameraComponent.h"
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
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippingWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
	DOREPLIFETIME(UCombatComponent, bIsFiring);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult HitResult;
	TraceUnderCrosshair(HitResult);
	SetHUDCrosshair(DeltaTime);
	InterpFOV(DeltaTime);
}

void UCombatComponent::SetAiming(bool bInAiming)
{
	if (!EquippingWeapon)
	{
		return;
	}
	
	bIsAiming = bInAiming;
	if (IsValidCharacter())
	{
		Character->SetADSWalkSpeed(bInAiming, ADSMultiplier);
	}
	
	ServerSetAiming(bInAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bInAiming)
{
	bIsAiming = bInAiming;
	if (IsValidCharacter())
	{
		Character->SetADSWalkSpeed(bInAiming, ADSMultiplier);
	}
}

void UCombatComponent::SetFiring(bool bInFiring)
{
	if (!EquippingWeapon)
	{
		return;
	}
	
	bIsFiring = bInFiring;
	
	FHitResult HitResult;
    TraceUnderCrosshair(HitResult);
    ServerFire(bInFiring, HitResult.ImpactPoint);
	if (bIsFiring)
	{
		CrosshairShootingFactor = 0.75f;
		
		// Debug Line
		// FTransform MuzzleTipTransform = EquippingWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), RTS_World);
		// FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		// DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red, false, 20.f, 0, 2.f);
		// DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), HitResult.ImpactPoint, FColor::Orange, false, 20.f, 0, 2.f);
	}
}

UAnimMontage* UCombatComponent::SelectHitReactMontage(const FVector& HitNormal)
{
	const FVector& ActorForward = GetOwner()->GetActorForwardVector();
	const FVector& ActorRight = GetOwner()->GetActorRightVector();

	const float ForwardHit = FVector::DotProduct(ActorForward, HitNormal);
	const float RightHit = FVector::DotProduct(ActorRight, HitNormal);

	UE_LOG(LogTemp, Warning, TEXT("ForwardHit %f , RightHit %f"), ForwardHit, RightHit);

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

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValidCharacter())
	{
		return;
	}
	if (Character->GetFollowCamera())
	{
		DefaultFOV = Character->GetFollowCamera()->FieldOfView;
		CurrentFOV = DefaultFOV;
	}
}

bool UCombatComponent::IsValidCharacter()
{
	if (!Character)
	{
		Character = Cast<ALBlasterCharacter>(GetOwner());
	}
	return Character != nullptr;
}

bool UCombatComponent::IsValidPlayerController()
{
	if (!PlayerController)
	{
		PlayerController = Cast<ALBlasterPlayerController>(Character->Controller);
	}
	return PlayerController != nullptr;
}

bool UCombatComponent::IsValidHUD()
{
	if (!HUD)
	{
		HUD = Cast<ALBlasterHUD>(PlayerController->GetHUD());
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
		if (IsValidCharacter())
		{
			const float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 50.f);
		}
		const FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility);

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
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
	if (!IsValidCharacter() || !IsValidPlayerController() || !IsValidHUD())
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
	if (Character->GetCharacterMovement())
	{
		const FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
		const FVector2D ClampRange(0.f, 1.f);
		const FVector Velocity = Character->GetCharacterMovement()->Velocity;
		const float CrosshairSpreadVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, ClampRange, Velocity.Size2D());

		if (Character->GetCharacterMovement()->IsFalling())
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

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (!EquippingWeapon || !IsValidCharacter())
	{
		return;
	}

	if (bIsAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippingWeapon->GetZoomedFOV(), DeltaTime, EquippingWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, EquippingWeapon->GetZoomInterpSpeed());
	}

	if (Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippingWeapon)
	{
		if (IsValidCharacter())
		{
			Character->SetWeaponAnimLayers(EquippingWeapon->GetWeaponAnimLayer());
		}
	}
}

void UCombatComponent::ServerFire_Implementation(bool bInFiring, const FVector_NetQuantize& TraceHitTarget)
{
	bIsFiring = bInFiring;

	if (bIsFiring)
	{
		MulticastFire(TraceHitTarget);
	}
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (IsValidCharacter())
	{
		Character->PlayFireMontage(FireMontages[EquippingWeapon->GetWeaponType()]);
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

		if (IsValidCharacter())
		{
			Character->AttachWeapon(EquippingWeapon);
			Character->SetWeaponAnimLayers(EquippingWeapon->GetWeaponAnimLayer());
		}
	}
}

