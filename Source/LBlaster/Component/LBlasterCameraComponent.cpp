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
	PenetrationBlendInTime = 0.1f;
	PenetrationBlendOutTime = 0.15f;
	CollisionPushOutDistance = 2.f;
	
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(0.f, 0.f, 0.f), 0.5f, 1.f, 5.f, 0));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(0.f, 0.f, 5.f), 0.2f, 0.75f, 0.f, 3));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(0.f, 0.f, -5.f), 0.2f, 0.75f, 0.f, 3));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(0.f, 0.f, 5.f), 0.15f, 0.5f, 0.f, 5));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(0.f, 0.f, -5.f), 0.15f, 0.5f, 0.f, 5));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(0.f, 15.f, 0.f), 0.5f, 1.f, 0.f, 4));
	PenetrationAvoidanceFeelers.Add(FPenetrationAvoidanceFeeler(FRotator(0.f, -10.f, 0.f), 0.5f, 0.5f, 0.f, 4));
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

	View.ControlRotation = InView.Rotation;

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

	UpdatePreventPenetration(DeltaTime, OutView);
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

void ULBlasterCameraComponent::UpdatePreventPenetration(float DeltaTime, FCameraView& OutView)
{
	if (const AActor* PPActor = OwnerCharacter)
	{
		if (const UPrimitiveComponent* PPActorRootComponent = Cast<UPrimitiveComponent>(PPActor->GetRootComponent()))
		{
			// 자동으로 SafeLocation을 선택해 카메라 이동을 최소화하려는 시도입니다.
			// 조준 라인에 가장 가까운 지점을 선택합니다.
			FVector ClosestPointOnLineToCapsuleCenter;
			FVector SafeLocation = PPActor->GetActorLocation();
			FMath::PointDistToLine(SafeLocation, OutView.Rotation.Vector(), OutView.Location, ClosestPointOnLineToCapsuleCenter);

			// 안전 거리를 조정하여 높이를 조준 라인과 동일하게 하되, 캡슐 내에 위치하도록 합니다.
			const float PushInDistance = PenetrationAvoidanceFeelers[0].Extent + CollisionPushOutDistance;
			const float MaxHalfHeight = PPActor->GetSimpleCollisionHalfHeight() - PushInDistance;
			SafeLocation.Z = FMath::Clamp(ClosestPointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight, SafeLocation.Z + MaxHalfHeight);

			// 초기에 카메라가 캐릭터 캡슐에 침투하는 것을 방지하기 위해 SafeLocation을 캡슐 내부로 밀어 넣습니다.	
			float DistanceSqr;
			PPActorRootComponent->GetSquaredDistanceToCollision(ClosestPointOnLineToCapsuleCenter, DistanceSqr, SafeLocation);
			SafeLocation += (SafeLocation - ClosestPointOnLineToCapsuleCenter).GetSafeNormal() * PushInDistance;
			
			PreventCameraPenetration(PPActor, SafeLocation, OutView.Location, DeltaTime);
		}
	}
}

void ULBlasterCameraComponent::	PreventCameraPenetration(const AActor* ViewTarget, FVector& SafeLocation, FVector& CameraLocation, float DeltaTime)
{
	float HardBlockedPct = DistBlockedPct;
	float SoftBlockedPct = DistBlockedPct;

	// 현재 카메라 위치와 SafeLocation 사이의 기본 Ray
	FVector BaseRay = CameraLocation - SafeLocation;
	FRotationMatrix BaseRayMatrix(BaseRay.Rotation());
	FVector BaseRayLocalUp, BaseRayLocalFwd, BaseRayLocalRight;
	BaseRayMatrix.GetScaledAxes(BaseRayLocalFwd, BaseRayLocalRight, BaseRayLocalUp);

	// 현재 프레임에서 차단된 비율
	float DistBlockedPctThisFrame = 1.f;
	
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActor(ViewTarget);
	
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(0.f);

	// 각 Feeler에 대해 충돌을 감지
	for (int32 RayIdx = 0; RayIdx < PenetrationAvoidanceFeelers.Num(); ++RayIdx)
	{
		FPenetrationAvoidanceFeeler& Feeler = PenetrationAvoidanceFeelers[RayIdx];

		if (Feeler.FramesUntilNextTrace <= 0)
		{
			// Feeler의 회전 값으로 Ray 회전
			FVector RotatedRay = BaseRay.RotateAngleAxis(Feeler.AdjustmentRot.Yaw, BaseRayLocalUp);
			RotatedRay = RotatedRay.RotateAngleAxis(Feeler.AdjustmentRot.Pitch, BaseRayLocalRight);
			FVector RayTarget = SafeLocation + RotatedRay;

			// Trace에 사용할 Sphere Radius 설정
			SphereShape.Sphere.Radius = Feeler.Extent;
			ECollisionChannel TraceChannel = ECC_Camera;

			// SafeLocation에서 Ray를 따라 Sphere로 Trace 
			FHitResult Hit;
			const bool bHit = GetWorld()->SweepSingleByChannel(Hit, SafeLocation, RayTarget, FQuat::Identity, TraceChannel, SphereShape, SphereParams);
			const AActor* HitActor = Hit.GetActor();

			// 해당 Feeler의 다음 Trace까지의 프레임 수 설정.
			// 현재 Trace에서 충돌이 감지되지 않으면 FramesUntilNextTrace만큼의 프레임이 지나야 다시 Trace 수행 가능
			Feeler.FramesUntilNextTrace = Feeler.TraceInterval;

			// 충돌 발생
			if (bHit && HitActor)
			{
				float const Weight = Cast<APawn>(Hit.GetActor()) ? Feeler.PawnWeight : Feeler.WorldWeight;
				float NewBlockPct = Hit.Time;
				NewBlockPct += (1.f - NewBlockPct) * (1.f - Weight);

				// CollisionPushOutDistance를 고려하여 차단 비율을 재계산
				// 차단 비율은 0에 가까울수록 SafeLocation에 가까움
				NewBlockPct = ((Hit.Location - SafeLocation).Size() - CollisionPushOutDistance) / (RayTarget - SafeLocation).Size();
				DistBlockedPctThisFrame = FMath::Min(NewBlockPct, DistBlockedPctThisFrame);

				// 충돌이 발생했으므로 다음 프레임에서 다시 추적
				Feeler.FramesUntilNextTrace = 0;
			}

			// 기존 카메라 위치로 향하는 Ray는 Interpolation X
			if (RayIdx == 0)
			{
				HardBlockedPct = DistBlockedPctThisFrame;
			}
			else
			{
				SoftBlockedPct = DistBlockedPctThisFrame;
			}
		}
		else
		{
			--Feeler.FramesUntilNextTrace;
		}
	}

	// 현재 프레임에서의 차단 비율이 더 큼 => 카메라가 SafeLocation 쪽으로 더 많이 침투한 상태 => 현재 차단 비율을 현재 프레임에서의 차단 비율이 되도록 설정해줌
	if (DistBlockedPct < DistBlockedPctThisFrame)
	{
		// 현재 프레임에서의 차단 비율까지 부드럽게 보간해 설정해줌.
		if (PenetrationBlendOutTime > DeltaTime)
		{
			DistBlockedPct = DistBlockedPct + DeltaTime / PenetrationBlendOutTime * (DistBlockedPctThisFrame - DistBlockedPct);
		}
		// 현재 차단 비율을 바로 현재 프레임에서의 차단 비율로 설정.
		else
		{
			DistBlockedPct = DistBlockedPctThisFrame;
		}
	}
	// 현재 차단 비율이 더 큼 => 카메라가 SafeLocation 쪽으로 덜 침투한 상태
	else
	{
		// 현재 차단 비율이 Base Ray에 의한 차단 비율보다 작다면 => 카메라가 더 뒤에 있음 => 바로 설정.
		if (DistBlockedPct > HardBlockedPct)
		{
			DistBlockedPct = HardBlockedPct;
		}
		// SoftBlockedPct는 Base Ray를 제외한 Feeler에서의 Trace에 의한 현재 프레임에서의 차단 비율
		else if (DistBlockedPct > SoftBlockedPct)
		{
			// 현재 차단 비율을 현재 프레임에서의 차단 비율로 보간해 부드럽게 설정
			if (PenetrationBlendInTime > DeltaTime)
			{
				DistBlockedPct = DistBlockedPct - DeltaTime / PenetrationBlendInTime * (DistBlockedPct - SoftBlockedPct);
			}
			// 현재 차단 비율을 바로 현재 프레임에서의 차단 비율로 설정.
			else
			{
				DistBlockedPct = SoftBlockedPct;
			}
		}
	}

	// 카메라가 SafeLocation 쪽으로 보간되고 있으면 보간된 위치로 설정
	DistBlockedPct = FMath::Clamp<float>(DistBlockedPct, 0.f, 1.f);
	if (DistBlockedPct < (1.f - ZERO_ANIMWEIGHT_THRESH))
	{
		CameraLocation = SafeLocation + (CameraLocation - SafeLocation) * DistBlockedPct;
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