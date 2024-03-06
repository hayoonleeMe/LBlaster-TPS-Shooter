// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RocketMovementComponent.h"

UProjectileMovementComponent::EHandleBlockingHitResult URocketMovementComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick,
	const FVector& MoveDelta, float& SubTickTimeRemaining)
{
	Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
	// Super::HandleBlockingHit()의 EHandleBlockingHitResult::Deflect 대신 AdvanceNextSubstep를 반환  
	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void URocketMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	// Super::HandleImpact()는 Projectile의 Simulation을 Stop하는데, ProjectileRocket은 OnHit 이벤트 발생 3초 이후 사라질 것이므로 Super::HandleImpact()를 호출하지 않게한다. 
}
