// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "InventoryComponent.h"
#include "Components/ChildActorComponent.h"
#include "GunBase.h"
#include "TP_WeaponComponent.generated.h"

class AScifiFPSCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCIFIFPS_API UTP_WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	/*UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AScifiFPSProjectile> ProjectileClass;*/

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/* Seconds to wait between shots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TimeBetweenShots;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	/* Is currently equipped gun automatic */
	bool IsAutomatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	/* Is the assault rifle the current weapon */
	bool AssaultRifleActive;

	/** Line trace distance (how far the player can shoot)*/
	float ShootingDistance;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SwitchAmmoAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SwitchWeaponsAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<UChildActorComponent*> GunArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	class UChildActorComponent* PrimaryGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	class UChildActorComponent* SecondaryGun;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	AGunBase* PrimaryGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	AGunBase* SecondaryGun;*/

public:
	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AScifiFPSCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchAmmoType();

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	/* Send a line trace for raycast shooting */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void RaycastShot();

	UFUNCTION()
	void SwitchWeapons(const FInputActionValue& index);

	void SwitchToNextWeapon();

protected:
	UFUNCTION()
	virtual void BeginPlay();

	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	AScifiFPSCharacter* Character;

	/* Shooting timer handle */
	FTimerHandle m_handleRefire;

	/* Automatic weapon ammo counter */
	uint32 m_automaticGunAmmoCount;

	/* Index for the gun the character is currently using */
	uint32 m_weaponIndex;

};
