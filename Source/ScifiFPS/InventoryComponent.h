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

	void ConsumeAmmo(EAmmunitionType ammo);

	uint32 GetAmmoCount(EAmmunitionType ammo) const;

	void ReloadWeapon(EAmmunitionType ammo);
private:
	TMap<EAmmunitionType, uint8> m_ammunitionCount;
	TMap<EAmmunitionType, uint8> m_maxAmmunition;

	FAmmunition m_ammunitionSettings;

};
