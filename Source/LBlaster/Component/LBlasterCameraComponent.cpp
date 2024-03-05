// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LBlasterCameraComponent.h"

#include "Character/LBlasterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ULBlasterCameraComponent::ULBlasterCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	bUsePawnControlRotation = false;	// 카메라는 항상 RelativeRotation을 유지해 캐릭터를 바라본다.
	PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	PostProcessSettings.DepthOfFieldFocalDistance = 10000.f;
	PostProcessSettings.bOverride_DepthOfFieldFstop = true;
	PostProcessSettings.DepthOfFieldFstop = 32.f;
	FieldOfView = 80.f;

	CameraMode = BaseMode;
}

void ULBlasterCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	if (!IsValidOwnerCharacter() || bIsDead)
	{
		return;
	}
	
	// Choose Camera Mode
	CameraMode = OwnerCharacter->IsAiming() ? ADSMode : BaseMode;

	// Update Camera Mode
	FCameraView UpdatedView;
	UpdateView(DeltaTime, UpdatedView);
	UpdateBlending(DeltaTime);

	// Blend View
	BlendView(UpdatedView);

	// Apply View
	ApplyView();
}

bool ULBlasterCameraComponent::IsValidOwnerCharacter()
{
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(GetOwner());
	}
	return OwnerCharacter != nullptr;
}

void ULBlasterCameraComponent::BlendView(const FCameraView& InView)
{
	View.Location = FMath::Lerp(View.Location, InView.Location, BlendWeight);

	const FRotator DeltaRotation = (InView.Rotation - View.Rotation).GetNormalized();
	View.Rotation += BlendWeight * DeltaRotation;

	const FRotator DeltaControlRotation = (InView.ControlRotation - View.ControlRotation).GetNormalized();
	View.ControlRotation += BlendWeight * DeltaControlRotation;

	View.FieldOfView = FMath::Lerp(View.FieldOfView, InView.FieldOfView, BlendWeight);
}

void ULBlasterCameraComponent::ApplyView()
{
	if (OwnerCharacter->IsLocallyControlled())
	{
		if (APlayerController* OwnerController = Cast<APlayerController>(OwnerCharacter->Controller))
		{
			OwnerController->SetControlRotation(View.ControlRotation);
		}
	}
	
	SetWorldLocationAndRotation(View.Location, View.Rotation);
	FieldOfView = View.FieldOfView;
}

FVector ULBlasterCameraComponent::GetPivotLocation() const
{
	const ACharacter* CharacterCDO = OwnerCharacter->GetClass()->GetDefaultObject<ACharacter>();
	check(CharacterCDO);

	const UCapsuleComponent* CapsuleComp = OwnerCharacter->GetCapsuleComponent();
	check(CapsuleComp);

	const UCapsuleComponent* CapsuleCDO = CharacterCDO->GetCapsuleComponent();
	check(CapsuleCDO);

	const float DefaultHalfHeight = CapsuleCDO->GetUnscaledCapsuleHalfHeight();
	const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
	const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + CharacterCDO->BaseEyeHeight;

	return OwnerCharacter->GetActorLocation() + (FVector::UpVector * HeightAdjustment);
}

FRotator ULBlasterCameraComponent::GetPivotRotation() const
{
	return OwnerCharacter->GetViewRotation();
}

void ULBlasterCameraComponent::UpdateView(float DeltaTime, FCameraView& OutView)
{
	UpdateForTarget(DeltaTime);
	UpdateCrouchOffset(DeltaTime);
	
	FVector PivotLocation = GetPivotLocation() + CurrentCrouchOffset;
	FRotator PivotRotation = GetPivotRotation();
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, CameraMode.ViewPitchMin, CameraMode.ViewPitchMax);

	if (UCurveVector* TargetOffsetCurve = CameraMode.OffsetCurve)
	{
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		PivotLocation += PivotRotation.RotateVector(TargetOffset);
	}

	OutView.Location = PivotLocation;
	OutView.Rotation = PivotRotation;
	OutView.ControlRotation = OutView.Rotation;
	OutView.FieldOfView = CameraMode.FieldOfView;
}

void ULBlasterCameraComponent::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.f, 1.f);

	const float InvExponent = CameraMode.BlendExponent > 0.f ? (1.f / CameraMode.BlendExponent) : 1.f;
	BlendAlpha = FMath::InterpEaseOut(0.f, 1.f, BlendWeight, InvExponent);
}

void ULBlasterCameraComponent::UpdateBlending(float DeltaTime)
{
	if (CameraMode.BlendTime > 0.f)
	{
		BlendAlpha += DeltaTime / CameraMode.BlendTime;
		BlendAlpha = FMath::Min(BlendAlpha, 1.f);
	}
	else
	{
		BlendAlpha = 1.f;
	}

	const float Exponent = CameraMode.BlendExponent > 0.f ? CameraMode.BlendExponent : 1.f;
	BlendWeight = FMath::InterpEaseOut(0.f, 1.f, BlendAlpha, Exponent);
}

void ULBlasterCameraComponent::UpdateForTarget(float DeltaTime)
{
	if (OwnerCharacter->bIsCrouched)
	{
		const float CrouchADSOffset = OwnerCharacter->IsAiming() ? -30.f : 0.f;
		const float CrouchMovingOffset = OwnerCharacter->GetCharacterMovement()->Velocity.Size2D() > 0.f ? 20.f : 0.f;
		const ACharacter* CDO = OwnerCharacter->GetClass()->GetDefaultObject<ACharacter>();
		const float CrouchedHeightAdjustment = CDO->CrouchedEyeHeight - CDO->BaseEyeHeight + CrouchADSOffset + CrouchMovingOffset;

		SetTargetCrouchOffset(FVector(0.f, 0.f, CrouchedHeightAdjustment));
	}
	else
	{
		SetTargetCrouchOffset(FVector::ZeroVector);
	}	
}

void ULBlasterCameraComponent::SetTargetCrouchOffset(const FVector& NewTargetOffset)
{
	CrouchOffsetBlendPct = 0.f;
	InitialCrouchOffset = CurrentCrouchOffset;
	TargetCrouchOffset = NewTargetOffset;
}

void ULBlasterCameraComponent::UpdateCrouchOffset(float DeltaTime)
{
	if (CrouchOffsetBlendPct < 1.0f)
	{
		CrouchOffsetBlendPct = FMath::Min(CrouchOffsetBlendPct + DeltaTime * CrouchOffsetBlendMultiplier, 1.0f);
		CurrentCrouchOffset = FMath::InterpEaseInOut(InitialCrouchOffset, TargetCrouchOffset, CrouchOffsetBlendPct, 1.0f);
	}
	else
	{
		CurrentCrouchOffset = TargetCrouchOffset;
		CrouchOffsetBlendPct = 1.0f;
	}
}