// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	FBoxInformation()
		: Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator), BoxExtent(FVector::ZeroVector)
	{}

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

	FFramePackage()
		: Time(0.f)
	{}

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	FServerSideRewindResult()
		: bHitConfirmed(false), bHeadShot(false), ImpactNormal(FVector_NetQuantizeNormal::ZeroVector), HitDistanceMeter(0.f)
	{}

	FServerSideRewindResult(bool bInHitConfirmed, bool bInHeadShot)
		: bHitConfirmed(bInHitConfirmed), bHeadShot(bInHeadShot), ImpactNormal(FVector_NetQuantizeNormal::ZeroVector), HitDistanceMeter(0.f)
	{}

	FServerSideRewindResult(bool bInHitConfirmed, bool bInHeadShot, FVector_NetQuantizeNormal InImpactNormal, float InHitDistanceMeter)
		: bHitConfirmed(bInHitConfirmed), bHeadShot(bInHeadShot), ImpactNormal(InImpactNormal), HitDistanceMeter(InHitDistanceMeter)
	{}

	UPROPERTY()
	uint8 bHitConfirmed : 1;

	UPROPERTY()
	uint8 bHeadShot : 1;

	UPROPERTY()
	FVector_NetQuantizeNormal ImpactNormal;

	UPROPERTY()
	float HitDistanceMeter;
};

USTRUCT()
struct FShotgunHitInfo
{
	GENERATED_BODY()

	FShotgunHitInfo()
		: HitCount(0), HitDistanceMeter(0.f)
	{}

	FShotgunHitInfo(uint32 InHitCount, float InHitDistanceMeter)
		: HitCount(InHitCount), HitDistanceMeter(InHitDistanceMeter)
	{}

	UPROPERTY()
	uint32 HitCount;

	UPROPERTY()
	float HitDistanceMeter;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<class ALBlasterCharacter*, FShotgunHitInfo> HeadShots;

	UPROPERTY()
	TMap<ALBlasterCharacter*, FShotgunHitInfo> BodyShots;
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
