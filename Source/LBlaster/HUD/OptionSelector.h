// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionSelector.generated.h"

/**
 * 
 */
UCLASS()
class LBLASTER_API UOptionSelector : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
public:
	void InitializeOptions();

	UFUNCTION()
	void SelectRightOption();

	UFUNCTION()
	void SelectLeftOption();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LBlaster", meta=(AllowPrivateAccess="true"))
	TArray<FString> Options;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> Switcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> LeftButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RightButton;

	int32 GetNewIndex(int32 InAddAmount) const;
};
