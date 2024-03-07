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

	void SetADSFOV(float InFOV);

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
	void UpdateForTarget(float DeltaTime);
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
};
