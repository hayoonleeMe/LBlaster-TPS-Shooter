// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LagCompensationComponent.h"

#include "Character/LBlasterCharacter.h"
#include "Components/BoxComponent.h"
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

void ULagCompensationComponent::SaveFramePackage(FFramePackage& OutPackage)
{
	if (IsValidOwnerCharacter() && GetWorld())
	{
		OutPackage.Time = GetWorld()->GetTimeSeconds();
		OutPackage.Character = OwnerCharacter;
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
		// too laggy to do SSR
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

FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage& Package, ALBlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation) const
{
	if (!HitCharacter)
	{
		return FServerSideRewindResult();
	}

	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);
	MoveBoxes(HitCharacter, Package);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);	// 캐릭터 메시에 Trace 방지

	// 머리 먼저
	UBoxComponent* HeadBox = HitCharacter->GetHitCollisionBoxes()[FName(TEXT("head"))];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	if (UWorld* World = GetWorld())
	{
		FHitResult ConfirmHitResult;
		World->LineTraceSingleByChannel(ConfirmHitResult, TraceStart, TraceEnd, ECC_Visibility);

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
					HitBoxPair.Value->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
				}
			}

			World->LineTraceSingleByChannel(ConfirmHitResult, TraceStart, TraceEnd, ECC_Visibility);
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

void ULagCompensationComponent::MoveBoxes(ALBlasterCharacter* HitCharacter, const FFramePackage& Package) const
{
	if (!HitCharacter)
	{
		return;
	}

	for (const TTuple<FName, UBoxComponent*>& HitBoxPair : HitCharacter->GetHitCollisionBoxes())
	{
		if (HitBoxPair.Value)
		{
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);
		}
	}
}

void ULagCompensationComponent::ResetHitBoxes(ALBlasterCharacter* HitCharacter, const FFramePackage& Package) const
{
	if (!HitCharacter)
	{
		return;
	}

	for (const TTuple<FName, UBoxComponent*>& HitBoxPair : HitCharacter->GetHitCollisionBoxes())
	{
		if (HitBoxPair.Value)
		{
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);
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




