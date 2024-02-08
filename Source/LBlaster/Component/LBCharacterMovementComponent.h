// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LBCharacterMovementComponent.generated.h"

/**
 * FLyraCharacterGroundInfo
 *
 *	Information about the ground under the character.  It only gets updated as needed.
 */
USTRUCT(BlueprintType)
struct FLBlasterCharacterGroundInfo
{
	GENERATED_BODY()

	FLBlasterCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

/**
 * 
 */
UCLASS()
class LBLASTER_API ULBCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	ULBCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
	
	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "LBlaster|CharacterMovement")
	const FLBlasterCharacterGroundInfo& GetGroundInfo();

protected:
	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FLBlasterCharacterGroundInfo CachedGroundInfo;
};
