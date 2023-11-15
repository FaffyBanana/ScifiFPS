// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TP_WeaponComponent.h"
#include "HealthComponent.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ScifiFPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

/*****************************************************************************************************
 * Type: Class
 *
 * Name: AScifiFPSCharacter
 *
 * Author: Jaber Ahmed
 *
 * Purpose: The player class, handles all player data
 *
 * References: Heavily edited version of Unreal's first player character template
 *
 * See Also: N/A
 *
 * Change Log:
 * Date          Initials    Version     Comments
 * 02/08/2023    JA          V1.0        N/A
*****************************************************************************************************/
UCLASS(config=Game)
class AScifiFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/* Constructor */
	AScifiFPSCharacter();

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(const bool bNewHasRifle);

	/** Return m_bhasrifle */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle() const;

	/* Return Health Component */
	UHealthComponent* GetHealthComponent() const;

	/* Return Inventory Component */
	UInventoryComponent* GetInventoryComponent() const;

	/* Return Weapon Component */
	UTP_WeaponComponent* GetWeaponComponent() const;

	/** Return Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Return FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Return WeaponPlacementComponent subobject **/
	USceneComponent* GetWeaponPlacementComponent() const { return WeaponPlacementComponent;  }

private:
	/* Begin Play event */
	virtual void BeginPlay();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for sprint input */
	void Sprint();

	/** Called for sprint input */
	void StopSprint();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// End of APawn interface
	virtual void Tick(float DeltaTime) override;

	/* Respawn and reset the player */
	void Respawn();

private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/* Scene component for where the weapon is placed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponPlacementComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/** Weapon component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	UTP_WeaponComponent* WeaponComponent;

	/** Health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;
	
	/** Inventory Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	bool m_hasRifle;

	/* Default Walk Speed */
	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	float m_defaultWalkSpeed;

	/* Default Sprint speed*/
	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	float m_defaultSprintSpeed;

	/* Game Over Menu */
	TSubclassOf<UUserWidget> GameOverWidgetClass;
	UUserWidget* GameOverWidget;

	/*TSubclassOf<UUserWidget> PlayerHUDWidgetClass;
	UUserWidget* PlayerHUDWidget;*/

	
};

