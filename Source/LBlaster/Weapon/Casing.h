// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class LBLASTER_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	ACasing();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	/*
	 *	Mesh
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CasingMesh;

	/*
	 *	Physics
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Physics")
	float ShellEjectionImpulse;

	/*
	 *	Sound
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Sound")
	TObjectPtr<USoundBase> ShellSound;

	bool bHitGround = false;

	/*
	 *	Destroy
	 */
	UPROPERTY(EditAnywhere, Category="LBlaster|Destroy")
	float DestroyTime = 2.f;

	void DestroySelf();
};
