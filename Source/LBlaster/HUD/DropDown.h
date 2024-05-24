// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchModeTypes.h"
#include "LBlasterUserWidget.h"
#include "DropDown.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UDropDown : public ULBlasterUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void InitializeOptions(const TArray<FString>& Options);
	int32 GetSelectedValue() const;

	inline const static TArray<FString> TeamDeathMatchNumMaxPlayerOptions{ TEXT("2"), TEXT("4"), TEXT("6"), TEXT("8"), TEXT("10"), TEXT("12") };
	inline const static TArray<FString> FreeForAllNumMaxPlayerOptions{ TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("5"), TEXT("6"), TEXT("7"), TEXT("8"), TEXT("9"), TEXT("10"), TEXT("11"), TEXT("12") };
	inline const static TArray<FString> GoalKillScoreOptions{ TEXT("10"), TEXT("20"), TEXT("40"), TEXT("60"), TEXT("80"), TEXT("100") };

private:
	UPROPERTY(EditAnywhere, Category="LBlaster")
	TSubclassOf<class UDropDownOption> DropDownOptionClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UComboBoxString> ComboBox;

	UFUNCTION()
	UWidget* OnComboBoxGenerateWidgetEvent(FString Item);
};
