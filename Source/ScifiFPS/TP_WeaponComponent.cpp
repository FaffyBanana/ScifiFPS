// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "ScifiFPSCharacter.h"
#include "ScifiFPSProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAIManager.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	// Default line trace distance
	ShootingDistance = 2000.0f;

	// Set time between shots
	TimeBetweenShots = 0.15f;

	/* Weapon defaults */
	IsAutomatic = true;  // Move this to gun specific
	m_weaponIndex = 0;
	PrimaryGun = CreateDefaultSubobject<UChildActorComponent>(TEXT("PrimaryWeapon"));
	SecondaryGun = CreateDefaultSubobject<UChildActorComponent>(TEXT("SecondaryWeapon"));

	m_currentWeapon = EAmmunitionType::AE_Primary;
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryComponent = GetOwner()->FindComponentByClass<UInventoryComponent>();

	if (PrimaryGun)
	{
		m_gunArray.Add(PrimaryGun);
		m_isWeaponActiveMap.Add(EAmmunitionType::AE_Primary, false);
	}

	if (SecondaryGun)
	{
		m_gunArray.Add(SecondaryGun);
		m_isWeaponActiveMap.Add(EAmmunitionType::AE_Secondary, false);
	}

	m_isWeaponActiveMap[m_currentWeapon] = true;

}

void UTP_WeaponComponent::AttachWeapon(AScifiFPSCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	/* Setup weapon attachment */
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	PrimaryGun->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	SecondaryGun->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &UTP_WeaponComponent::StartFire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::StopFire);
			EnhancedInputComponent->BindAction(SwitchAmmoAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::SwitchAmmoType);
			EnhancedInputComponent->BindAction(SwitchWeaponsAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::SwitchWeapons);
		}
	}
}

void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	if (InventoryComponent)
	{
		if (InventoryComponent->GetAmmoCount(m_currentWeapon) > 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(InventoryComponent->GetAmmoCount(m_currentWeapon))); // DEBUG DELETE -------------------------
			
			InventoryComponent->ConsumeAmmo(m_currentWeapon);

			// Try and play a firing animation if specified
			if (FireAnimation != nullptr)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
				if (AnimInstance != nullptr)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}

			RaycastShot();
		}
		else
		{
			InventoryComponent->ReloadWeapon(m_currentWeapon);
			Fire(); // TODO: Move to this to reload animation timer -------------
		}
	}
}

void UTP_WeaponComponent::RaycastShot()
{
	// Create variables for line trace
	FVector loc;
	FRotator rot;
	FHitResult hit;

	// Get players POV
	Character->GetController()->GetPlayerViewPoint(loc, rot);

	// Set variables
	FVector start = loc;
	FVector end = start + (rot.Vector() * ShootingDistance);

	// Send line trace
	FCollisionQueryParams traceParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, traceParams);

	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.0f); // DEBUG -----------------------

	// If line trace has hit an object
	if (bHit)
	{
		AEnemyAIManager* enemyManager = Cast<AEnemyAIManager>(hit.GetActor());
		if (enemyManager)
		{
			enemyManager->GetHealthComponent()->TakeDamage();
			//DrawDebugBox(GetWorld(), hit.ImpactPoint, FVector(5, 5, 5), FColor::Blue, false, 2.0f); // DEBUG -----------------------
		}
	}
}

void UTP_WeaponComponent::SwitchWeapons(const FInputActionValue& index)
{
	/* Calculate correct weapon index to switch to */
	float tempIndex = index.Get<float>();
	int tempValue = UKismetMathLibrary::FTrunc(tempIndex);
	int tempWeaponIndex = tempValue + m_weaponIndex;

	// If index exists is in array
	if (m_gunArray.IsValidIndex(tempWeaponIndex))
	{
		m_weaponIndex = tempWeaponIndex;
	}

	// If player has scrolled to an index that doesn't exist in the array
	else
	{
		/* If the player has scrolled down go back to top of the array,
			if player has scrolled up go back to the bottom of the array */
		m_weaponIndex < 0 ? m_weaponIndex = m_gunArray.Num() - 1 : m_weaponIndex = 0;
	}
	SwitchToNextWeapon();

}

void UTP_WeaponComponent::SwitchToNextWeapon()
{
	StopFire();

	// Set gun actors as invisible
	for (int i = 0; i < m_gunArray.Num(); i++)
	{
		m_gunArray[i]->SetVisibility(false);
		IsAutomatic = false;
	}

	for (TPair<EAmmunitionType, bool>& pair: m_isWeaponActiveMap)
	{
		pair.Value = false;
	}

	switch (m_weaponIndex)
	{
	/* Primary Weapon */
	case 0:
		
		m_currentWeapon = EAmmunitionType::AE_Primary;
		IsAutomatic = true;
		break;
	
	/* Secondary Weapon*/
	case 1:
		
		m_currentWeapon = EAmmunitionType::AE_Secondary;
		IsAutomatic = false;
		break;

	///* Tertiary Weapon */
	//case 2:
	//	if (GunArray.Num() > 3)
	//	{
	//		m_weaponIndex = 3;
	//		SwitchWeapons(m_weaponIndex);
	//	}
	//	else
	//	{
	//		m_weaponIndex = 0;
	//		SwitchWeapons(m_weaponIndex);
	//	}
	//	break;

	default:
		break;
	}

	// Set current weapon to be visible
	m_gunArray[m_weaponIndex]->SetVisibility(true);
	m_isWeaponActiveMap[m_currentWeapon] = true;

}

void UTP_WeaponComponent::StartFire()
{
	/* Fire weapon */
	Fire();
	
	if (IsAutomatic)
		/* Automatic shooting timer */
		Character->GetWorldTimerManager().SetTimer(m_handleRefire, this, &UTP_WeaponComponent::Fire, TimeBetweenShots, true);
}

void UTP_WeaponComponent::StopFire()
{
	if (IsAutomatic)
		/* Clear automatic shooting timer */
		Character->GetWorldTimerManager().ClearTimer(m_handleRefire);
}

void UTP_WeaponComponent::SwitchAmmoType()
{
	StopFire();
	IsAutomatic = !IsAutomatic;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}


/// Code from default template that I don't need but don't want to delete yet

//// Try and fire a projectile
	//if (ProjectileClass != nullptr)
	//{
	//	UWorld* const World = GetWorld();
	//	if (World != nullptr)
	//	{
	//		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	//		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	//		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	//		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	//
	//		//Set Spawn Collision Handling Override
	//		FActorSpawnParameters ActorSpawnParams;
	//		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	//
	//		// Spawn the projectile at the muzzle
	//		World->SpawnActor<AScifiFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	//	}
	//}

	// Try and play the sound if specified
	//if(FireSound != nullptr)
	//{
		//UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	//}

	