// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/AmmoPickup.h"

#include "Components/SphereComponent.h"
#include "Interface/LBCharacterPickupInterface.h"
#include "Kismet/GameplayStatics.h"

AAmmoPickup::AAmmoPickup()
{
	// 모든 Actor 클래스에 상속되는 기본 USceneComponent
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	/* Mesh & Overlap Sphere */
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(90.f);
	OverlapSphere->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(OverlapSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
	PickupMesh->SetRenderCustomDepth(true);
	PickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
}

void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PickupMesh)
	{
		PickupMesh->AddWorldRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
	}
}

void AAmmoPickup::Destroyed()
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
	
	Super::Destroyed();
}

void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
}

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                  bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (ILBCharacterPickupInterface* Interface = Cast<ILBCharacterPickupInterface>(OtherActor))
		{
			Interface->PickupAmmo(WeaponType, AmmoAmount);
		}
	}

	Destroy();
}
