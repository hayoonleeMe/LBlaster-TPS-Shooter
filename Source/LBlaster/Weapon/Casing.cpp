// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Casing.h"

#include "Kismet/GameplayStatics.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	/* Mesh */
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Casing Mesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	/* Physics */
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulse = 10.f;
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound && !bHitGround)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
		bHitGround = true;
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::DestroySelf, DestroyTime);
}

void ACasing::DestroySelf()
{
	Destroy();
}
