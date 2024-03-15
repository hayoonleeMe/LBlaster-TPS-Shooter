// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class LBLASTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Destroyed() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*
	 *	Auto Turn
	 */	
	UPROPERTY(EditAnywhere, Category="LBlaster|Auto Turn")
	float BaseTurnRate;
	
private:
	/*
	 *	Mesh & Overlap Sphere
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> OverlapSphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	/*
	 *	Sound
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Sound")
	TObjectPtr<USoundBase> PickupSound;
};
