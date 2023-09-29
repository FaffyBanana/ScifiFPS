// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EAmmunitionType : uint8 
{
	AE_Primary UMETA(DisplayName = "PrimaryAmmunition"),
	AE_Secondary UMETA(DisplayName = "AE_SecondaryAmmunition"),
};

USTRUCT(BlueprintType)
struct FAmmunition
{
	GENERATED_BODY()

public:
	UPROPERTY()
	uint8 DefaultMaxPrimaryAmmunition = 30;

	UPROPERTY()
	uint8 DefaultSecondaryAmmunition = 30;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCIFIFPS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Consume ammunition by decrementing count */
	void ConsumeAmmo(EAmmunitionType ammo);

	/* Return the ammunition count */
	uint32 GetAmmoCount(EAmmunitionType ammo) const;

	/* Reloads the weapon by resetting the count back to it's max ammunition */
	void ReloadWeapon(EAmmunitionType ammo);

private:
	// Hash map for ammunition count of each weapon
	TMap<EAmmunitionType, uint8> m_ammunitionCount;

	// Hash map for the max amount of ammunition (before gun has to reload the catridge) of each weapon
	TMap<EAmmunitionType, uint8> m_maxAmmunition;

	// Reference to struct that holds the ammunitions default variables 
	FAmmunition m_ammunitionSettings;

};
