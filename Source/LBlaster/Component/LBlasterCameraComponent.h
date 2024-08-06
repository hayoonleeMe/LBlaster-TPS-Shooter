// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "LBlasterCameraComponent.generated.h"

USTRUCT(BlueprintType)
struct FCameraView
{
	GENERATED_BODY()

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

USTRUCT(BlueprintType)
struct FCameraMode
{
	GENERATED_BODY()

	FCameraMode()
	{
		ViewPitchMin = -89.f;
		ViewPitchMax = 89.f;
		FieldOfView = 80.f;
		BlendExponent = 4.f;
		BlendTime = 0.5f;
	}

	// Minimum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	// Maximum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	TObjectPtr<class UCurveVector> OffsetCurve;

	// Exponent used by blend functions to control the shape of the curve.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	// How long it takes to blend in this mode.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;
};

/**
 * Struct defining a feeler ray used for camera penetration avoidance.
 */
USTRUCT()
struct FPenetrationAvoidanceFeeler
{
	GENERATED_BODY()

	/** 메인 레이에서의 편차를 나타내는 FRotator */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	FRotator AdjustmentRot;

	/** how much this feeler affects the final position if it hits the world */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float WorldWeight;

	/** how much this feeler affects the final position if it hits a APawn (setting to 0 will not attempt to collide with pawns at all) */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float PawnWeight;

	/** 이 Feeler로 Trace를 할 때 사용할 Sphere의 Radius */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float Extent;

	/** 이전 프레임에서 충돌이 없었을 때 이 Ray(Feeler)가 다시 추적되기까지의 최소 프레임 간격을 나타냅니다. */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	int32 TraceInterval;

	/** 이 Ray(Feeler)가 마지막으로 사용된 이후의 프레임 수를 나타내며, 일시적으로 사용됩니다. */
	UPROPERTY(transient)
	int32 FramesUntilNextTrace;

	FPenetrationAvoidanceFeeler()
		: AdjustmentRot(ForceInit)
		  , WorldWeight(0)
		  , PawnWeight(0)
		  , Extent(0)
		  , TraceInterval(0)
		  , FramesUntilNextTrace(0)
	{
	}

	FPenetrationAvoidanceFeeler(const FRotator& InAdjustmentRot,
	                            const float& InWorldWeight,
	                            const float& InPawnWeight,
	                            const float& InExtent,
	                            const int32& InTraceInterval = 0,
	                            const int32& InFramesUntilNextTrace = 0)
		: AdjustmentRot(InAdjustmentRot)
		  , WorldWeight(InWorldWeight)
		  , PawnWeight(InPawnWeight)
		  , Extent(InExtent)
		  , TraceInterval(InTraceInterval)
		  , FramesUntilNextTrace(InFramesUntilNextTrace)
	{
	}
};

/**
 * 
 */
UCLASS()
class LBLASTER_API ULBlasterCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	ULBlasterCameraComponent(const FObjectInitializer& ObjectInitializer);
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	void SetBlendWeight(float Weight);
	bool bIsDead = false;

	void SetAdsFov(float InFov);

private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwnerCharacter;

	bool IsValidOwnerCharacter();

	/*
	 *	View
	 */
	FCameraView View;
	void UpdateView(float DeltaTime, FCameraView& OutView);
	void UpdateForTarget();
	void UpdateCrouchOffset(float DeltaTime);
	void BlendView(const FCameraView& InView);
	void ApplyView();

	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;
	void SetTargetCrouchOffset(const FVector& NewTargetOffset);

	float CrouchOffsetBlendMultiplier = 5.0f;
	FVector InitialCrouchOffset = FVector::ZeroVector;
	FVector TargetCrouchOffset = FVector::ZeroVector;
	float CrouchOffsetBlendPct = 1.0f;
	FVector CurrentCrouchOffset = FVector::ZeroVector;

	/*
	 *	Camera Mode
	 */
	FCameraMode CameraMode;

	UPROPERTY(EditAnywhere, Category="LBlaster|CameraMode")
	FCameraMode BaseMode;

	UPROPERTY(EditAnywhere, Category="LBlaster|CameraMode")
	FCameraMode ADSMode;

	/*
	 *	Blending
	 */
	void UpdateBlending(float DeltaTime);

	// Linear blend alpha used to determine the blend weight.
	float BlendAlpha = 1.f;

	// Blend weight calculated using the blend alpha and function.
	float BlendWeight = 1.f;

	/*
	 *	Prevent Penetration
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Prevent Penetration")
	float PenetrationBlendInTime;

	UPROPERTY(EditAnywhere, Category="LBlaster|Prevent Penetration")
	float PenetrationBlendOutTime;
	
	UPROPERTY(EditAnywhere, Category = "LBlaster|Prevent Penetration")
	float CollisionPushOutDistance;

	UPROPERTY(EditDefaultsOnly, Category = "LBlaster|Prevent Penetration")
	TArray<FPenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;
	
	float DistBlockedPct = 0.f;
	
	void UpdatePreventPenetration(float DeltaTime, FCameraView& OutView);
	void PreventCameraPenetration(const AActor* ViewTarget, FVector& SafeLocation, FVector& CameraLocation, float DeltaTime);
};
