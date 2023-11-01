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
	m_weaponIndex = 0;
	m_currentWeapon = EAmmunitionType::AE_Primary;

	/* Find the blueprint Primary Gun Class through reference */
	static ConstructorHelpers::FClassFinder<AGunBase> PrimaryWeaponFinder(TEXT("/Game/FirstPerson/Blueprints/Weapon/BP_AutomaticRifle"));
	if (PrimaryWeaponFinder.Class)
	{
		PrimaryWeaponRef = PrimaryWeaponFinder.Class;
	}

	/* Find the blueprint Secondary Gun Class through reference */
	static ConstructorHelpers::FClassFinder<AGunBase> SecondaryWeaponFinder(TEXT("/Game/FirstPerson/Blueprints/Weapon/BP_SingleShotRifle"));
	if (SecondaryWeaponFinder.Class)
	{
		SecondaryWeaponRef = SecondaryWeaponFinder.Class;
	}

	m_bCanShoot = true;
	m_bIsFiring = false;

}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryComponent = GetOwner()->FindComponentByClass<UInventoryComponent>();


	/* Spawn all guns */
	FVector location(0.0f, 0.0f, 0.0f);
	FRotator rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters spawnInfo;
	PrimaryGun = GetWorld()->SpawnActor<AGunBase>(PrimaryWeaponRef, location, rotation, spawnInfo);
	SecondaryGun = GetWorld()->SpawnActor<AGunBase>(SecondaryWeaponRef, location, rotation, spawnInfo);

	/* Set primary gun defaults */
	if (PrimaryGun)
	{
		m_gunArray.Add(PrimaryGun);
		m_isWeaponActiveMap.Add(EAmmunitionType::AE_Primary, false);
		m_isAutomaticMap.Add(EAmmunitionType::AE_Primary, true);
	}

	/* Set secondary gun defaults */
	if (SecondaryGun)
	{
		m_gunArray.Add(SecondaryGun);
		m_isWeaponActiveMap.Add(EAmmunitionType::AE_Secondary, false);
		m_isAutomaticMap.Add(EAmmunitionType::AE_Secondary, false);
	}

	/* Set current weapon as active */
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
	PrimaryGun->GetGunSkeletalMeshComponent()->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	SecondaryGun->GetGunSkeletalMeshComponent()->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

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
			EnhancedInputComponent->BindAction(SwitchWeaponsAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::SwitchWeapons);
		}
	}

	SwitchToNextWeapon();
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

void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	if (InventoryComponent)
	{
		// If the player has ammunition and can shoot
		if (!ShouldPlayerReload() && m_bCanShoot)
		{
			m_bIsFiring = true;

			// Decrement ammunition counter of current weapon
			InventoryComponent->ConsumeAmmo(m_currentWeapon);

			// Shoot raycast line
			RaycastShot();

			// Try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
			}
		}

		// If the player has no ammunition 
		if (ShouldPlayerReload() && m_bCanShoot)
		{
			//// Try and play a reload animation if specified
			//if (ReloadAnimation != nullptr)
			//{
			//	// Get the animation object for the arms mesh
			//	UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
			//	if (AnimInstance != nullptr)
			//	{
			//		AnimInstance->Montage_Play(ReloadAnimation, 1.0f);
			//	}
			//}

			StartReloadWeaponTimer();
		}
	}
}

void UTP_WeaponComponent::StartReloadWeaponTimer()
{
	// Stop the player from shooting
	m_bCanShoot = false;

	// Start reload timer
	Character->GetWorldTimerManager().SetTimer(m_handleReload, this, &UTP_WeaponComponent::ReloadWeapon, 1.0f, true); // TODO: Change timer to however long the animation takes
}

void UTP_WeaponComponent::ClearReloadWeaponTimer()
{
	// Clear reload timer
	Character->GetWorldTimerManager().ClearTimer(m_handleReload);
}

bool UTP_WeaponComponent::ShouldPlayerReload() const
{
	// Has the player run out of ammunition 
	return InventoryComponent->GetAmmoCount(m_currentWeapon) == 0 ?  true : false;
}

void UTP_WeaponComponent::ReloadWeapon()
{
	// Calculate the ammunition counter of the weapon after reload
	InventoryComponent->ReloadWeapon(m_currentWeapon);
	
	// Clear the reload timer
	ClearReloadWeaponTimer();

	// Allow the player to shoot again
	m_bCanShoot = true;
}


void UTP_WeaponComponent::SwitchWeapons(const FInputActionValue& index)
{
	ClearReloadWeaponTimer();

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

	StopFire();

	ShouldPlayerReload() ? StartReloadWeaponTimer() : m_bCanShoot = true;
}

int32 UTP_WeaponComponent::GetCurrentAmmo() const
{
	return InventoryComponent->GetAmmoCount(m_currentWeapon);
}

int32 UTP_WeaponComponent::GetTotalCurrentAmmo() const
{
	return InventoryComponent->GetTotalAmmoCount(m_currentWeapon);
}

bool UTP_WeaponComponent::GetIsFiring() const
{
	return m_bIsFiring;
}

void UTP_WeaponComponent::SwitchToNextWeapon()
{
	/* Set gun actors as invisible */
	for (int i = 0; i < m_gunArray.Num(); i++)
	{
		m_gunArray[i]->GetGunSkeletalMeshComponent()->SetVisibility(false);
	}

	/* Set all gun as inactive */
	for (TPair<EAmmunitionType, bool>& pair: m_isWeaponActiveMap)
	{
		pair.Value = false;
	}

	switch (m_weaponIndex)
	{
	/* Primary Weapon */
	case 0:
		
		m_currentWeapon = EAmmunitionType::AE_Primary;
		break;
	
	/* Secondary Weapon*/
	case 1:
		
		m_currentWeapon = EAmmunitionType::AE_Secondary;
		break;

	default:
		break;
	}

	// Set current weapon to be visible and active
	m_gunArray[m_weaponIndex]->GetGunSkeletalMeshComponent()->SetVisibility(true);
	m_isWeaponActiveMap[m_currentWeapon] = true;

}

void UTP_WeaponComponent::StartFire()
{

	// Fire weapon 
	Fire();
	
	if (m_isAutomaticMap[m_currentWeapon])
		// Automatic shooting timer 
		Character->GetWorldTimerManager().SetTimer(m_handleRefire, this, &UTP_WeaponComponent::Fire, TimeBetweenShots, true);
}

void UTP_WeaponComponent::StopFire()
{
	//if (m_isAutomaticMap[m_currentWeapon])
	// Clear automatic shooting timer 
	Character->GetWorldTimerManager().ClearTimer(m_handleRefire);
	m_bIsFiring = false;
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



// Code from default template that I don't need but don't want to delete yet
	// Try and fire a projectile
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
	//
	// Try and play the sound if specified
	//if(FireSound != nullptr)
	//{
		//UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	//}

	