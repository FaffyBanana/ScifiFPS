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
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachWeapon(AScifiFPSCharacter* TargetCharacter);

	/* Calculate the correct weapon index */
	UFUNCTION()
	void SwitchWeapons(const FInputActionValue& index);

	/* Get the current ammunition count of the current weapon */
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() const;

	/* Get the total ammunition count of the current weapon */
	UFUNCTION(BlueprintCallable)
	int32 GetTotalCurrentAmmo() const;

	/* Get boolean to see if player is firing */
	UFUNCTION(BlueprintCallable)
	bool GetIsFiring() const;

	/* Get the boolean to see if player is reloading */
	UFUNCTION(BlueprintCallable)
	bool GetIsReloading() const;

	UFUNCTION(BlueprintCallable)
	bool GetIsAimingIn() const;

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

	/* Handles the reloading of the current weapon */
	void ReloadWeapon();

	/* Start the reload weapon timer, also sets booleans to stop shooting and start reloading */
	void StartReloadWeaponTimer();

	/* Clear the reload weapon timer */
	void ClearReloadWeaponTimer();

	/* Evaluate if the player should reload or not */
	bool ShouldPlayerReload() const;

	/* Play sound effects for shooting the weapon */
	void PlayGunShotSFX();

	/* Aim down scope of gun */
	void AimInSight();

	/* Aim out of scope to go back to hip fire */
	void AimOutSight();

public:
	/* Seconds to wait between shots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TimeBetweenShots;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

private:
	/** Line trace distance (how far the player can shoot)*/
	float ShootingDistance;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	/** Aiming (ADS) Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimDownSightAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwitchWeaponsAction;

	/* Primary gun */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AGunBase* PrimaryGun;

	/* Secondary gun */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AGunBase* SecondaryGun;
	
private:
	// The Character holding this weapon 
	AScifiFPSCharacter* Character;

	// Inventory component reference
	UInventoryComponent* InventoryComponent;

	// Shooting timer handle 
	FTimerHandle m_handleRefire;

	// Index for the gun the character is currently using 
	int32 m_weaponIndex;

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

	/* If player can shoot */
	bool m_bCanShoot;

	/* Is player firing weapons (Used for animations) */
	bool m_bIsFiring;

	/* Bool for if the player is reloading or not
					(Used for animations) */
	bool m_bIsReloading;

	bool m_bIsAimingIn;

	/* How long it takes to reload */
	float m_reloadTime;

	/* Handle for reload timer */
	FTimerHandle m_handleReload;
};

	// /** Sound to play each time we fire */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	//USoundBase* FireSound;
	//
	// /** AnimMontage to play each time we fire */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	//UAnimMontage* FireAnimation;
	//
	// /** AnimMontage to play each time we reload */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	//UAnimMontage* ReloadAnimation;