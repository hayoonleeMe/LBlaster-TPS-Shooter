// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 bHitConfirmed : 1;

	UPROPERTY()
	uint8 bHeadShot : 1;

	UPROPERTY()
	FVector_NetQuantizeNormal ImpactNormal;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<class ALBlasterCharacter*, uint32> HeadShots;

	UPROPERTY()
	TMap<ALBlasterCharacter*, uint32> BodyShots;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LBLASTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULagCompensationComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& InPackage, const FColor& InColor) const;
	FServerSideRewindResult ServerSideRewind(class ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);
	FShotgunServerSideRewindResult ShotgunServerSideRewind(const TArray<ALBlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime);
	FServerSideRewindResult ProjectileServerSideRewind(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime, float GravityScale);

	FORCEINLINE bool IsServerSideRewindEnabled() const { return bEnableServerSideRewind; }
	void EnableServerSideRewind(bool bInEnabled);
	
protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& OutPackage);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(ALBlasterCharacter* HitCharacter, float HitTime);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	FServerSideRewindResult ConfirmHit(const FFramePackage& InPackage, ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation) const;
	FShotgunServerSideRewindResult ShotgunConfirmHit(const TArray<FFramePackage>& InPackages, const TArray<ALBlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations) const;
	FServerSideRewindResult ProjectileConfirmHit(const FFramePackage& InPackage, ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime, float GravityScale);
	void CacheBoxPositions(ALBlasterCharacter* HitCharacter, FFramePackage& OutPackage) const;
	void MoveBoxes(ALBlasterCharacter* HitCharacter, const FFramePackage& InPackage) const;
	void ResetHitBoxes(ALBlasterCharacter* HitCharacter, const FFramePackage& InPackage) const;
	void EnableCharacterMeshCollision(ALBlasterCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled) const;

private:
	UPROPERTY(EditAnywhere, Category="LBlaster")
    bool bEnableServerSideRewind;
	
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwnerCharacter;

	bool IsValidOwnerCharacter();

	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> OwnerController;

	bool IsValidOwnerController();

	/*
	 *	Frame History
	 */
	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere, Category="LBlaster|Frame History")
	float MaxRecordTime;
};
