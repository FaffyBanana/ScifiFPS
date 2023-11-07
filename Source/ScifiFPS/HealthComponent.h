// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCIFIFPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	int32 GetHealth() const;

	UFUNCTION(BlueprintCallable)
	int32 GetMaxHealth() const;

	UFUNCTION(BlueprintCallable)
	void TakeDamage();

	UFUNCTION(BlueprintCallable)
	void ResetHealth();

	UHealthComponent* GetHealthComponent() const;

private:
	int32 m_health; // Current health
	int32 m_maxHealth; // Maximum health amount
		
};
