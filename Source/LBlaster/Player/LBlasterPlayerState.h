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
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddToKillScore(int32 InKillScoreAmount);
	void AddToDeath(int32 InDeathAmount);
	
	FORCEINLINE void SetKillScore(int32 InKillScoreAmount) { KillScore = InKillScoreAmount; }
	FORCEINLINE int32 GetKillScore() const { return KillScore; }
	FORCEINLINE int32 GetDeath() const { return Death; }

	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE void SetTeam(ETeam InTeam) { Team = InTeam; }
	void InitTeam();
	void InitTeamFromGameInstance();
	FTeamCharacterMaterials GetCharacterMaterials() const;

protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;
	
private:
	/*
	 *	Core
	 */
	bool bFirstTimeInit = false;
	void PollInit();

	/*
	 *	Owner
	 */
	UPROPERTY()
	TObjectPtr<class ALBlasterCharacter> OwnerCharacter;

	bool IsValidOwnerCharacter();

	UPROPERTY()
	TObjectPtr<class ALBlasterPlayerController> OwnerController;
	
	bool IsValidOwnerController();

	/*
	 *	Kill Score
	 */
	UPROPERTY(ReplicatedUsing=OnRep_KillScore)
	int32 KillScore;

	UFUNCTION()
	void OnRep_KillScore();
	
	/*
	 *	Death
	 */
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
