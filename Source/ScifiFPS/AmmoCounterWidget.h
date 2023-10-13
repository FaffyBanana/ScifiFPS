// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "ScifiFPSCharacter.h"
#include "AmmoCounterWidget.generated.h"

/**
 * 
 */
UCLASS()
class SCIFIFPS_API UAmmoCounterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/* Native Construct */
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	FText GetAmmoText() const;

	UFUNCTION(BlueprintCallable)
	FText GetTotalAmmoText() const;
private:

public:
	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* HorizontalBox;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AmmoTotalText;

private:
	AScifiFPSCharacter* Player;
};
