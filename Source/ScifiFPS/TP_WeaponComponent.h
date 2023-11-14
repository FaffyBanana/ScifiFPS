// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "InventoryComponent.h"
#include "Components/ChildActorComponent.h"
#include "GunBase.h"
#include "Components/TimelineComponent.h"
#include "TP_WeaponComponent.generated.h"

class AScifiFPSCharacter;
class UCurveFloat;

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
 * 02/08/2023    JA          v1.0        Weapon Firing
 * 27/09/2023	 JA			 v1.1		 Weapon Switching
 * 06/11/2023	 JA			 v1.2		 Reload weapon
 * 13/11/2023	 JA			 v1.3		 Implemented ADS
*****************************************************************************************************/

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCIFIFPS_API UTP_WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/* Get the current ammunition count of the current weapon from inventory component */
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmoOfCurrentWeapon() const;

	/* Get the total ammunition count of the current weapon from inventory component */
	UFUNCTION(BlueprintCallable)
	int32 GetReserveAmmoOfCurrentWeapon() const;

	/* Get boolean to see if player is firing */
	UFUNCTION(BlueprintCallable)
	bool GetIsFiring() const;

	/* Get the boolean to see if player is reloading */
	UFUNCTION(BlueprintCallable)
	bool GetIsReloading() const;

	/* Get the boolean to see if player is aiming in */
	UFUNCTION(BlueprintCallable)
	bool GetIsAimingIn() const;

protected:
	/* Seconds to wait between shots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TimeBetweenShots;

	/** Line trace distance (how far the player can shoot) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float ShootingDistance;

	UPROPERTY(EditAnywhere, Category = Timeline)
	UCurveFloat* ADSCurveFloat; // Curve float for aiming down sights

private:
	/** Begin play event */
	UFUNCTION()
	virtual void BeginPlay();

	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void AttachWeapon();

	/* Calculate the correct weapon index */
	UFUNCTION()
	void SwitchWeapons(const FInputActionValue& index);

	/* Set weapon defaults */
	void InitWeapons();

	/* Set aim down site curve and timeline defaults */
	void InitADSTimeline();

	/* Get the maximum ammount the current weapon can reload up to */
	int32 GetMaxAmmoCatridgeOfCurrentWeapon() const;

	/* Aim down scope of gun */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void AimInSight();

	/* Aim out of scope to go back to hip fire */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void AimOutSight();

	UFUNCTION()
	void AimInTimelineProgress(float value); // Called when the ADS timeline is played

	/* Play the timeline for aiming down sights */
	void PlayADSTimeline();

	/* Reverse the timeline for aiming down sights */
	void ReverseADSTimeline();

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

	/* Spawn weapon using transform and reference */
	AGunBase* SpawnWeapon(const TSubclassOf<AGunBase> weaponRef, const FTransform transform, const FActorSpawnParameters spawnInfo);

	/* Spawn weapon using location, rotation and reference */
	AGunBase* SpawnWeapon(const TSubclassOf<AGunBase> weaponRef, const FVector location, const FRotator rotator, const FActorSpawnParameters spawnInfo);

private:
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
	AGunBase* PrimaryWeapon;

	/* Secondary gun */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AGunBase* SecondaryWeapon;

	UPROPERTY()
	UTimelineComponent* ADSCurveTimeline; // Timeline component for aiming down sights

	/* Location of primary gun ADS location */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	//FVector MeshADSLocation; 
	
	// The Character holding this weapon 
	AScifiFPSCharacter* Character;

	// Inventory component reference
	UInventoryComponent* InventoryComponent;

	// Shooting timer handle 
	FTimerHandle m_handleRefire;

	// Index for the gun the character is currently using 
	int32 m_weaponIndex;

	//Array that holds all weapons (guns) as child actors 
	TArray<AGunBase*> m_weaponArray;

	/* Ammunition mappings */
	TMap<EAmmunitionType, bool> m_isWeaponActiveMap; // Which weapon is active currently
	TMap<EAmmunitionType, bool> m_isAutomaticMap; // Which weapons are automatic 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmunitionType, FVector> MeshArmsADSLocation;
	// Holds what weapon is currently active
	EAmmunitionType m_currentWeapon; 

	/* References to weapons */
	TSubclassOf<AGunBase> PrimaryWeaponRef;
	TSubclassOf<AGunBase> SecondaryWeaponRef;

	/* If player can shoot */
	bool m_bCanShoot;

	/* Is player firing weapons (Used for animations) */
	bool m_bIsFiring;

	/* Bool for if the character is reloading or not
					(Used for animations) */
	bool m_bIsReloading;

	/* Bool for if the character is aiming in or not */
	bool m_bIsAimingIn;

	/* How long it takes to reload */
	float m_reloadTime;

	/* Handle for reload timer */
	FTimerHandle m_handleReload;

	/* Default placement of weapon */
	FVector m_weaponPlacementLocation;

	/* Default placement of mesh arms */
	FVector m_meshPlacementLocation;

	/* Aiming down sights defaults */
	float m_aDSFOV;
	float m_aDSDistanceToCamera;

	/* FOV of camera */
	float m_fOV;
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