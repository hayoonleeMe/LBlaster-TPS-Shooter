// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LagCompensationComponent.h"

#include "Character/LBlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "Player/LBlasterPlayerController.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	/* Frame History */
	MaxRecordTime = 4.f;
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& InPackage, const FColor& InColor) const
{
	for (const TTuple<FName, FBoxInformation>& BoxInfo : InPackage.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(), BoxInfo.Value.Location, BoxInfo.Value.BoxExtent, FQuat(BoxInfo.Value.Rotation), InColor, false, MaxRecordTime);
	}
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




