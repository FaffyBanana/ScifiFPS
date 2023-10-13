// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "InventoryComponent.h"
#include "Components/ChildActorComponent.h"
#include "GunBase.h"
#include "TP_WeaponComponent.generated.h"


class AScifiFPSCharacter;

/*****************************************************************************************************
 * Type: Class
 *
 * Name: UTP_WeaponComponent
 *
 * Author: Jaber Ahmed
 *
 * Purpose: Handles all weapon data including guns and shooting
 *
 * References: Heavily edited version of Unreal's TP_WeaponComponent template 
 *
 * See Also: UInventoryComponent (ammunition storage and consumption)
 *
 * Change Log:
 * Date          Initials    Version     Comments
 * 02/08/2023    JA          v1.0        N/A
*****************************************************************************************************/

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCIFIFPS_API UTP_WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AScifiFPSCharacter* TargetCharacter);

	/* Calculate the correct weapon index */
	UFUNCTION()
	void SwitchWeapons(const FInputActionValue& index);

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() const;

protected:
	UFUNCTION()
	virtual void BeginPlay();

	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/* Fire single shot */
	void Fire();

	/* Start shooting
	   Sets timer for automatic firing */
	void StartFire();

	/* Stop shooting
	   Clears timer for automatic firing */
	void StopFire();

	/* Create line trace for shooting */
	void RaycastShot();

	/* Handles the switching of weapons */
	void SwitchToNextWeapon();

public:
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/* Seconds to wait between shots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TimeBetweenShots;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	/** Line trace distance (how far the player can shoot)*/
	float ShootingDistance;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwitchWeaponsAction;

	/* Primary gun */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	AGunBase* PrimaryGun;

	/* Secondary gun */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	AGunBase* SecondaryGun;
	
private:
	// The Character holding this weapon 
	AScifiFPSCharacter* Character;

	// Inventory component reference
	UInventoryComponent* InventoryComponent;

	// Shooting timer handle 
	FTimerHandle m_handleRefire;

	// Index for the gun the character is currently using 
	uint32 m_weaponIndex;

	//Array that holds all weapons (guns) as child actors 
	TArray<AGunBase*> m_gunArray;

	/* Ammunition mappings */
	TMap<EAmmunitionType, bool> m_isWeaponActiveMap; // Which weapon is active currently
	TMap<EAmmunitionType, bool> m_isAutomaticMap; // Which weapons are automatic 

	// Holds what weapon is currently active
	EAmmunitionType m_currentWeapon; 

	/* References to weapons */
	TSubclassOf<AGunBase> PrimaryWeaponRef;
	TSubclassOf<AGunBase> SecondaryWeaponRef;

};
