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
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	/*
	 *	Spawn Pickup
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Spawn Pickup")
	TArray<TSubclassOf<class APickup>> PickupClasses;

	UPROPERTY(ReplicatedUsing=OnRep_SpawnedPickup)
	TObjectPtr<APickup> SpawnedPickup;

	UFUNCTION()
	virtual void OnRep_SpawnedPickup();

	virtual void SpawnPickup();

	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere, Category="LBlaster|Spawn Pickup")
	float SpawnCooldownTime;

	FTimerHandle SpawnPickupTimer;

	/*
	 *	Pad Mesh
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PadMesh;

	/*
	 *	Pad Particle
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Pad Particle")
	TObjectPtr<class UNiagaraSystem> PadParticle;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> PadParticleComponent;

	UPROPERTY(EditAnywhere, Category="LBlaster|Pad Particle")
	TObjectPtr<UNiagaraSystem> PadLoadingParticle;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> PadLoadingParticleComponent;

	UPROPERTY(EditAnywhere, Category="LBlaster|Pad Particle")
	TObjectPtr<UNiagaraSystem> PadPickupParticle;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> PadPickupParticleComponent;

	UPROPERTY(EditAnywhere, Category="LBlaster|Pad Particle")
	FLinearColor PadColor;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastActivatePadLoadingParticle();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeactivatePadLoadingParticle();
};
