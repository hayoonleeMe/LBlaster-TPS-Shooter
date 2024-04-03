// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LagCompensationComponent.h"

#include "LBlaster.h"
#include "Character/LBlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LBlasterPlayerController.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bEnableServerSideRewind = true;

	/* Frame History */
	MaxRecordTime = 4.f;
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SaveFramePackage();
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& InPackage, const FColor& InColor) const
{
	for (const TTuple<FName, FBoxInformation>& BoxInfo : InPackage.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(), BoxInfo.Value.Location, BoxInfo.Value.BoxExtent, FQuat(BoxInfo.Value.Rotation), InColor, false, MaxRecordTime);
	}
}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize& HitLocation, float HitTime)
{
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);

	// FrameToCheck Confirm Hit
	return ConfirmHit(FrameToCheck, HitCharacter, TraceStart, HitLocation);
}
FShotgunServerSideRewindResult ULagCompensationComponent::ShotgunServerSideRewind(const TArray<ALBlasterCharacter*>& HitCharacters,
	const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	TArray<FFramePackage> FramesToCheck;
	for (ALBlasterCharacter* HitCharacter : HitCharacters)
	{
		FramesToCheck.Emplace(GetFrameToCheck(HitCharacter, HitTime));
	}

	return ShotgunConfirmHit(FramesToCheck, HitCharacters, TraceStart, HitLocations);
}

FServerSideRewindResult ULagCompensationComponent::ProjectileServerSideRewind(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize100& InitialVelocity, float HitTime, float GravityScale)
{
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);

	// FrameToCheck Confirm Hit
	return ProjectileConfirmHit(FrameToCheck, HitCharacter, TraceStart, InitialVelocity, HitTime, GravityScale);
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& OutPackage)
{
	if (IsValidOwnerCharacter() && GetWorld())
	{
		OutPackage.Time = GetWorld()->GetTimeSeconds();
		for (const TTuple<FName, UBoxComponent*>& BoxPair : OwnerCharacter->GetHitCollisionBoxes())
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			OutPackage.HitBoxInfo.Emplace(BoxPair.Key, BoxInformation);
		}
	}
}

void ULagCompensationComponent::SaveFramePackage()
{
	if (FrameHistory.Num() > 1)
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
	}
	FFramePackage ThisFrame;
	SaveFramePackage(ThisFrame);
	FrameHistory.AddHead(ThisFrame);
}

FFramePackage ULagCompensationComponent::GetFrameToCheck(ALBlasterCharacter* HitCharacter, float HitTime)
{
	if (!HitCharacter || !HitCharacter->GetLagCompensationComponent() || HitCharacter->GetLagCompensationComponent()->FrameHistory.IsEmpty())
	{
		return FFramePackage();
	}

	// Frame Package that we check to verify hit
	FFramePackage FrameToCheck;
	FrameToCheck.Time = HitTime;

	// Hit Character FrameHistory
	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensationComponent()->FrameHistory;
	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;

	// HitTime이 FrameHistory 범위 안에 있을 때
	if (OldestHistoryTime < HitTime && HitTime < NewestHistoryTime)
	{
		TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
		TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;
		while (Older->GetValue().Time >= HitTime)
		{
			// March back until: OlderTime < HitTime < YoungerTime
			if (!Older->GetNextNode())
			{
				break;
			}
			Older = Older->GetNextNode();
			if (Older->GetValue().Time > HitTime)
			{
				Younger = Older;
			}
		}

		// 정확히 찾음
		if (Older->GetValue().Time == HitTime)
		{
			FrameToCheck = Older->GetValue();
		}
		else
		{
			// Interpolate between Younger and Older
			FrameToCheck = InterpBetweenFrames(Older->GetValue(), Younger->GetValue(), HitTime);
		}		
	}
	else
	{
		// too Laggy to do SSR
		if (OldestHistoryTime > HitTime)
		{
			return FFramePackage();
		}
		if (OldestHistoryTime == HitTime)
		{
			FrameToCheck = History.GetTail()->GetValue();
		}
		if (NewestHistoryTime <= HitTime)
		{
			FrameToCheck = History.GetHead()->GetValue();
		}
	}

	return FrameToCheck;
}

FFramePackage ULagCompensationComponent::InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime)
{
	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float InterpFraction = FMath::Clamp((HitTime - OlderFrame.Time) / Distance, 0.f, 1.f);

	FFramePackage InterpFramePackage;
	InterpFramePackage.Time = HitTime;

	for (const TTuple<FName, FBoxInformation>& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerPair.Key;

		const FBoxInformation& OlderBox = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBox = YoungerFrame.HitBoxInfo[BoxInfoName];

		FBoxInformation InterpBoxInfo;
		InterpBoxInfo.Location = FMath::VInterpTo(OlderBox.Location, YoungerBox.Location, 1.f, InterpFraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBox.Rotation, YoungerBox.Rotation, 1.f, InterpFraction);
		InterpBoxInfo.BoxExtent = YoungerBox.BoxExtent;

		InterpFramePackage.HitBoxInfo.Emplace(BoxInfoName, InterpBoxInfo);
	}

	return InterpFramePackage;
}

FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage& InPackage, ALBlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation) const
{
	if (!HitCharacter)
	{
		return FServerSideRewindResult();
	}

	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);
	MoveBoxes(HitCharacter, InPackage);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);	// 캐릭터 메시에 Trace 방지

	// 머리 먼저
	UBoxComponent* HeadBox = HitCharacter->GetHitCollisionBoxes()[FName(TEXT("head"))];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);

	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	if (UWorld* World = GetWorld())
	{
		FHitResult ConfirmHitResult;
		World->LineTraceSingleByChannel(ConfirmHitResult, TraceStart, TraceEnd, ECC_HitBox);

		// hit head
		if (ConfirmHitResult.bBlockingHit)
		{
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult{ true, true };
		}
		// 다른 박스 체크
		else
		{
			for (const TTuple<FName, UBoxComponent*>& HitBoxPair : HitCharacter->GetHitCollisionBoxes())
			{
				if (HitBoxPair.Value != nullptr)
				{
					HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
				}
			}

			World->LineTraceSingleByChannel(ConfirmHitResult, TraceStart, TraceEnd, ECC_HitBox);
			if (ConfirmHitResult.bBlockingHit)
			{
				ResetHitBoxes(HitCharacter, CurrentFrame);
				EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
				return FServerSideRewindResult{ true, false };
			}
		}
	}

	// No Hit
	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult{ false, false };
}

FShotgunServerSideRewindResult ULagCompensationComponent::ShotgunConfirmHit(const TArray<FFramePackage>& InPackages, const TArray<ALBlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations) const
{
	if (InPackages.Num() != HitCharacters.Num())
	{
		return FShotgunServerSideRewindResult();
	}
	for (ALBlasterCharacter* HitCharacter : HitCharacters)
	{
		if (!HitCharacter)
		{
			return FShotgunServerSideRewindResult();
		}
	}

	FShotgunServerSideRewindResult ShotgunResult;
	TArray<FFramePackage> CurrentFrames;
	for (int32 Index = 0; Index < InPackages. Num(); ++Index)
	{
		FFramePackage CurrentFrame;
		CacheBoxPositions(HitCharacters[Index], CurrentFrame);
		MoveBoxes(HitCharacters[Index], InPackages[Index]);
		EnableCharacterMeshCollision(HitCharacters[Index], ECollisionEnabled::NoCollision);
		CurrentFrames.Emplace(CurrentFrame);
	}

	if (CurrentFrames.Num() != InPackages.Num())
	{
		return FShotgunServerSideRewindResult();
	}

	// enable head box collision
	for (ALBlasterCharacter* HitCharacter : HitCharacters)
	{
		UBoxComponent* HeadBox = HitCharacter->GetHitCollisionBoxes()[FName(TEXT("head"))];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
	}

	// Head Check
	for (const FVector& HitLocation : HitLocations)
	{
		if (UWorld* World = GetWorld())
		{
			const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
			FHitResult ConfirmHitResult;
			World->LineTraceSingleByChannel(ConfirmHitResult, TraceStart, TraceEnd, ECC_HitBox);

			// hit head
			if (ConfirmHitResult.bBlockingHit && ConfirmHitResult.GetActor())
			{
				if (ALBlasterCharacter* HitCharacter = Cast<ALBlasterCharacter>(ConfirmHitResult.GetActor()))
				{
					if (ShotgunResult.HeadShots.Contains(HitCharacter))
					{
						++ShotgunResult.HeadShots[HitCharacter];
					}
					else
					{
						ShotgunResult.HeadShots.Emplace(HitCharacter, 1);
					}
				}
			}
		}
	}

	// enable body box collision, disable head box collision 
	for (ALBlasterCharacter* HitCharacter : HitCharacters)
	{
		for (const TTuple<FName, UBoxComponent*>& HitBoxPair : HitCharacter->GetHitCollisionBoxes())
		{
			if (HitBoxPair.Value)
			{
				HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
			}
		}

		if (UBoxComponent* HeadBox = HitCharacter->GetHitCollisionBoxes()[FName("head")])
		{
			HeadBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	// Body Check
	for (const FVector& HitLocation : HitLocations)
	{
		if (UWorld* World = GetWorld())
		{
			const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
			FHitResult ConfirmHitResult;
			World->LineTraceSingleByChannel(ConfirmHitResult, TraceStart, TraceEnd, ECC_HitBox);

			// hit head
			if (ConfirmHitResult.bBlockingHit && ConfirmHitResult.GetActor())
			{
				if (ALBlasterCharacter* HitCharacter = Cast<ALBlasterCharacter>(ConfirmHitResult.GetActor()))
				{
					if (ShotgunResult.BodyShots.Contains(HitCharacter))
					{
						++ShotgunResult.BodyShots[HitCharacter];
					}
					else
					{
						ShotgunResult.BodyShots.Emplace(HitCharacter, 1);
					}
				}
			}
		}
	}

	// Reset
	for (int32 Index = 0; Index < InPackages.Num(); ++Index)
	{
		ResetHitBoxes(HitCharacters[Index], CurrentFrames[Index]);
		EnableCharacterMeshCollision(HitCharacters[Index], ECollisionEnabled::QueryAndPhysics);
	}

	return ShotgunResult;
}

FServerSideRewindResult ULagCompensationComponent::ProjectileConfirmHit(const FFramePackage& InPackage, ALBlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime, float GravityScale)
{
	if (!HitCharacter || !IsValidOwnerCharacter())
	{
		return FServerSideRewindResult();
	}

	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);
	MoveBoxes(HitCharacter, InPackage);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);	// 캐릭터 메시에 Trace 방지

	// 머리 먼저
	UBoxComponent* HeadBox = HitCharacter->GetHitCollisionBoxes()[FName(TEXT("head"))];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);

	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithCollision = true;
	PathParams.MaxSimTime = MaxRecordTime;
	PathParams.LaunchVelocity = InitialVelocity;
	PathParams.StartLocation = TraceStart;
	PathParams.SimFrequency = 15.f;	// 값이 클수록 포물선이 더 정확해짐 -> 비용이 커짐
	PathParams.ProjectileRadius = 5.f;
	PathParams.TraceChannel = ECC_HitBox;
	PathParams.ActorsToIgnore.Add(OwnerCharacter);
	// PathParams.DrawDebugTime = 5.f;
	// PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PathParams.OverrideGravityZ = GravityScale;
	
	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);

	// hit head, return early
	if (PathResult.HitResult.bBlockingHit)
	{
		ResetHitBoxes(HitCharacter, CurrentFrame);
		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
		return FServerSideRewindResult{ true, true, PathResult.HitResult.ImpactNormal };
	}
	
	// 다른 박스 체크
	for (const TTuple<FName, UBoxComponent*>& HitBoxPair : HitCharacter->GetHitCollisionBoxes())
	{
		if (HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);	
		}
	}
	
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
	if (PathResult.HitResult.bBlockingHit)
	{
		ResetHitBoxes(HitCharacter, CurrentFrame);
		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
		return FServerSideRewindResult{ true, false, PathResult.HitResult.ImpactNormal };
	}

	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult{ false, false };
}

void ULagCompensationComponent::CacheBoxPositions(ALBlasterCharacter* HitCharacter, FFramePackage& OutPackage) const
{
	if (!HitCharacter)
	{
		return;
	}

	for (const TTuple<FName, UBoxComponent*>& HitBoxPair : HitCharacter->GetHitCollisionBoxes())
	{
		if (HitBoxPair.Value)
		{
			FBoxInformation BoxInfo;
			BoxInfo.Location = HitBoxPair.Value->GetComponentLocation();
			BoxInfo.Rotation = HitBoxPair.Value->GetComponentRotation();
			BoxInfo.BoxExtent = HitBoxPair.Value->GetScaledBoxExtent();
			OutPackage.HitBoxInfo.Emplace(HitBoxPair.Key, BoxInfo);
		}
	}
}

void ULagCompensationComponent::MoveBoxes(ALBlasterCharacter* HitCharacter, const FFramePackage& InPackage) const
{
	if (!HitCharacter)
	{
		return;
	}

	for (const TTuple<FName, UBoxComponent*>& HitBoxPair : HitCharacter->GetHitCollisionBoxes())
	{
		if (HitBoxPair.Value)
		{
			HitBoxPair.Value->SetWorldLocation(InPackage.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(InPackage.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(InPackage.HitBoxInfo[HitBoxPair.Key].BoxExtent);
		}
	}
}

void ULagCompensationComponent::ResetHitBoxes(ALBlasterCharacter* HitCharacter, const FFramePackage& InPackage) const
{
	if (!HitCharacter)
	{
		return;
	}

	for (const TTuple<FName, UBoxComponent*>& HitBoxPair : HitCharacter->GetHitCollisionBoxes())
	{
		if (HitBoxPair.Value)
		{
			HitBoxPair.Value->SetWorldLocation(InPackage.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(InPackage.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(InPackage.HitBoxInfo[HitBoxPair.Key].BoxExtent);
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ULagCompensationComponent::EnableCharacterMeshCollision(ALBlasterCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled) const
{
	if (HitCharacter && HitCharacter->GetMesh())
	{
		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionEnabled);
	}
}

void ULagCompensationComponent::EnableServerSideRewind(bool bInEnabled)
{
	bEnableServerSideRewind = bInEnabled;
}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool ULagCompensationComponent::IsValidOwnerCharacter()
{
	if (!OwnerCharacter && GetOwner())
	{
		OwnerCharacter = Cast<ALBlasterCharacter>(GetOwner());
	}
	return OwnerCharacter != nullptr;
}

bool ULagCompensationComponent::IsValidOwnerController()
{
	if (!OwnerController && IsValidOwnerCharacter() && OwnerCharacter->GetController())
	{
		OwnerController = Cast<ALBlasterPlayerController>(OwnerCharacter->GetController());
	}
	return OwnerController != nullptr;
}




