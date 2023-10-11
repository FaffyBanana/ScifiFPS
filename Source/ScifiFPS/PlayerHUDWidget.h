// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoCounterWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class SCIFIFPS_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPlayerHUDWidget(const FObjectInitializer& ObjectInitalizer);

	/* Native Construct */
	virtual void NativeConstruct() override;
};
