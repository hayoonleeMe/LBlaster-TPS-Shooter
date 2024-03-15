// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class LBLASTER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();

protected:
	virtual void BeginPlay() override;

	/*
	 *	Spawn Pickup
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Spawn Pickup")
	TArray<TSubclassOf<class APickup>> PickupClasses;

	UPROPERTY()
	TObjectPtr<APickup> SpawnedPickup;

	void SpawnPickup();

	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere, Category="LBlaster|Spawn Pickup")
	float SpawnTime;

private:
	FTimerHandle SpawnPickupTimer;

	/*
	 *	Pad Mesh
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PadMesh;
};
