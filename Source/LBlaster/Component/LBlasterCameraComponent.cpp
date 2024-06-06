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

	/* Prevent Penetration */
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(+00.0f, +00.0f, 0.0f),  00.f));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(+00.0f, +16.0f, 0.0f),  00.f));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(+00.0f, -16.0f, 0.0f),  00.f));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(+00.0f, +32.0f, 0.0f),  00.f));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(+00.0f, -32.0f, 0.0f),  00.f));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(+20.0f, +00.0f, 0.0f),  00.f));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(-20.0f, +00.0f, 0.0f),  00.f));
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
	if (!OwnerCharacter && GetOwner())
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
	UpdateForTarget();
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

	UpdatePreventPenetration(OutView);
}

void ULBlasterCameraComponent::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.f, 1.f);

	const float InvExponent = CameraMode.BlendExponent > 0.f ? (1.f / CameraMode.BlendExponent) : 1.f;
	BlendAlpha = FMath::InterpEaseOut(0.f, 1.f, BlendWeight, InvExponent);
}

void ULBlasterCameraComponent::SetAdsFov(float InFov)
{
	ADSMode.FieldOfView = InFov;
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

void ULBlasterCameraComponent::UpdatePreventPenetration(FCameraView& OutView)
{
	if (const AActor* PPActor = OwnerCharacter)
	{
		if (const UPrimitiveComponent* PPActorRootComponent = Cast<UPrimitiveComponent>(PPActor->GetRootComponent()))
		{
			// 자동으로 SafeLocation을 선택하려는 시도입니다. 이를 통해 조준 시 카메라의 이동을 줄일 수 있습니다.
			// 우리의 카메라는 조준점이므로, 조준을 가능한 한 부드럽고 안정적으로 유지하고 싶습니다.
			// 캡슐에서 조준 라인에 가장 가까운 지점을 선택합니다.
			FVector ClosestPointOnLineToCapsuleCenter;
			FVector SafeLocation = PPActor->GetActorLocation();
			FMath::PointDistToLine(SafeLocation, OutView.Rotation.Vector(), OutView.Location, ClosestPointOnLineToCapsuleCenter);

			// 안전 거리를 조정하여 높이를 조준 라인과 동일하게 하되, 캡슐 내에 위치하도록 합니다.
			const float PushInDistance = PenetrationAvoidanceFeelers[0].Extent + CollisionPushOutDistance;
			const float MaxHalfHeight = PPActor->GetSimpleCollisionHalfHeight() - PushInDistance;
			SafeLocation.Z = FMath::Clamp(ClosestPointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight, SafeLocation.Z + MaxHalfHeight);

			float DistanceSqr;
			PPActorRootComponent->GetSquaredDistanceToCollision(ClosestPointOnLineToCapsuleCenter, DistanceSqr, SafeLocation);
			// 라인 검사를 수행할 때 초기 침투를 방지하기 위해 캡슐 내부로 밀어 넣습니다.	
			SafeLocation += (SafeLocation - ClosestPointOnLineToCapsuleCenter).GetSafeNormal() * PushInDistance;
			
			PreventCameraPenetration(PPActor, SafeLocation, OutView.Location);
		}
	}
}

void ULBlasterCameraComponent::PreventCameraPenetration(const AActor* PPActor, const FVector& SafeLocation, FVector& CameraLocation)
{
	if (PPActor)
	{
		FVector BaseRay = CameraLocation - SafeLocation;
		FRotationMatrix BaseRayMatrix(BaseRay.Rotation());
		FVector BaseRayLocalUp, BaseRayLocalFwd, BaseRayLocalRight;
	
		BaseRayMatrix.GetScaledAxes(BaseRayLocalFwd, BaseRayLocalRight, BaseRayLocalUp);
		float DistBlockedPctThisFrame = 1.f;
	
		FCollisionQueryParams SphereParams;
		SphereParams.AddIgnoredActor(PPActor);

		for (const FPenetrationAvoidanceFeeler& Feeler : PenetrationAvoidanceFeelers)
		{
			FVector RotatedRay = BaseRay.RotateAngleAxis(Feeler.AdjustmentRot.Yaw, BaseRayLocalUp);
			RotatedRay = RotatedRay.RotateAngleAxis(Feeler.AdjustmentRot.Pitch, BaseRayLocalRight);
			FVector RayTarget = SafeLocation + RotatedRay;

			FHitResult Hit;
			if (GetWorld()->SweepSingleByChannel(Hit, SafeLocation, RayTarget, FQuat::Identity, ECC_Camera, FCollisionShape::MakeSphere(Feeler.Extent), SphereParams))
			{
				float NewBlockPct = ((Hit.Location - SafeLocation).Size() - CollisionPushOutDistance) / (RayTarget - SafeLocation).Size();
				DistBlockedPctThisFrame = FMath::Min(NewBlockPct, DistBlockedPctThisFrame);
			}
		}

		DistBlockedPctThisFrame = FMath::Clamp(DistBlockedPctThisFrame, 0.f, 1.f);
		CameraLocation = SafeLocation + (CameraLocation - SafeLocation) * DistBlockedPctThisFrame;
	}
}

void ULBlasterCameraComponent::UpdateForTarget()
{
	if (!IsValidOwnerCharacter())
	{
		return;
	}
	
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