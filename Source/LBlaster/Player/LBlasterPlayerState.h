// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LBTypes/Team.h"
#include "LBlasterPlayerState.generated.h"

USTRUCT(BlueprintType)
struct FTeamCharacterMaterials
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInstance> MaterialInst1;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInstance> MaterialInst2;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInstance> DissolveInst1;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInstance> DissolveInst2;
};

/**
 * 
 */
UCLASS()
class LBLASTER_API ALBlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALBlasterPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	
	void AddToScore(float InScoreAmount);
	void AddToDeath(int32 InDeathAmount);

	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE void SetTeam(ETeam InTeam) { Team = InTeam; }
	void InitTeam();
	void InitTeamFromGameInstance();

	FTeamCharacterMaterials GetCharacterMaterials() const;
	
	FORCEINLINE int32 GetDeath() const { return Death; }

protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;
	
private:
	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwnerCharacter;

	bool IsValidOwnerCharacter();

	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> OwnerController;
	
	bool IsValidOwnerController();

	UPROPERTY(ReplicatedUsing=OnRep_Death)
	int32 Death;
	
	UFUNCTION()
    virtual void OnRep_Death();

	/*
	 *	Team
	 */
	UPROPERTY(BlueprintReadOnly, Replicated, meta=(AllowPrivateAccess = true))
	ETeam Team;

	UPROPERTY(EditAnywhere, Category="LBlaster")
	TMap<ETeam, FTeamCharacterMaterials> TeamCharacterMaterialsMap;

	UFUNCTION(Server, Reliable)
	void ServerSetTeam(ETeam InTeam);
};
