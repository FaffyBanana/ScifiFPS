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
#include "Components/TimelineComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
	:ShootingDistance (2000.0f) // Default line trace distance
	,TimeBetweenShots (0.15f) // Time between each bullet
	,m_weaponIndex (0) // Default equipped weapon index
	,m_currentWeapon (EAmmunitionType::AE_Primary) // Default equipped weapon type
	,m_bCanShoot (true)
	,m_bIsFiring (false)
	,m_bIsReloading (false)
	,m_bIsAimingIn (false)
	,m_reloadTime (2.25f)
{
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
	
	/* Get owning character of the component */
	Character = Cast<AScifiFPSCharacter>(GetOwner());

	//// Event tick defaults 
	//PrimaryComponentTick.bCanEverTick = true;
	//PrimaryComponentTick.bTickEvenWhenPaused = true;
	//PrimaryComponentTick.TickGroup = TG_PrePhysics; 
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryComponent = GetOwner()->FindComponentByClass<UInventoryComponent>();

	/* Spawn all guns */
	FTransform transform = Character->GetWeaponPlacementComponent()->GetComponentTransform();
	FActorSpawnParameters spawnInfo;
	PrimaryGun = GetWorld()->SpawnActor<AGunBase>(PrimaryWeaponRef, transform, spawnInfo);
	SecondaryGun = GetWorld()->SpawnActor<AGunBase>(SecondaryWeaponRef, transform, spawnInfo);

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

	m_weaponPlacementLocation = Character->GetWeaponPlacementComponent()->GetRelativeLocation();
	m_meshPlacementLocation = Character->GetMesh1P()->GetRelativeLocation();

	// ADS Curve
	if (ADSCurveFloat)
	{
		ADSCurveTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
		ADSCurveTimeline->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;
		Character->BlueprintCreatedComponents.Add(ADSCurveTimeline);

		FOnTimelineFloat onTimelineCallback;
		
		onTimelineCallback.BindUFunction(this, FName(TEXT("AimInTimelineProgress")));
		ADSCurveTimeline->AddInterpFloat(ADSCurveFloat, onTimelineCallback);
		ADSCurveTimeline->SetLooping(false);
		 
		StartLocation = Character->GetActorLocation();
		EndLocation = StartLocation;
		ADSCurveTimeline->RegisterComponent();

		m_aDSFOV = 75.0f;
		m_aDSDistanceToCamera = 10.0f;
	}


}

void UTP_WeaponComponent::AimInTimelineProgress(float value)
{
	float socketADS =  m_gunArray[m_weaponIndex]->GetGunSkeletalMeshComponent()->GetSocketTransform(FName(TEXT("ADS_Socket")), ERelativeTransformSpace::RTS_Actor).GetLocation().Y;
	FVector lerp(m_aDSDistanceToCamera, 0.0f, socketADS);

	FVector lerpLocation = UKismetMathLibrary::VLerp(m_weaponPlacementLocation, lerp, value);

	//m_gunArray[m_weaponIndex]->SetActorRelativeLocation(lerpLocation);
	Character->GetWeaponPlacementComponent()->SetRelativeLocation(lerpLocation);

	FVector meshADSLocation(-16.000000, 1.450000, -165.700000); // For primary weapon

	float tempADSFOV = UKismetMathLibrary::Lerp(90, m_aDSFOV, value);
	Character->GetFirstPersonCameraComponent()->SetFieldOfView(tempADSFOV);

	FVector meshLerp = UKismetMathLibrary::VLerp(m_meshPlacementLocation, MeshADSLocation, value);
	Character->GetMesh1P()->SetRelativeLocation(meshLerp);

}

void UTP_WeaponComponent::PlayADSTimeline()
{
	if (ADSCurveTimeline)
	{
		ADSCurveTimeline->PlayFromStart();
	}
}

void UTP_WeaponComponent::ReverseADSTimeline()
{
	if (ADSCurveTimeline)
	{
		ADSCurveTimeline->Reverse();
	}
}

void UTP_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//CurveTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
}

void UTP_WeaponComponent::AttachWeapon()
{
	/*Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}*/
	
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
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::StartReloadWeaponTimer);

			EnhancedInputComponent->BindAction(AimDownSightAction, ETriggerEvent::Started, this, &UTP_WeaponComponent::AimInSight);
			EnhancedInputComponent->BindAction(AimDownSightAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::AimOutSight);
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
			DrawDebugBox(GetWorld(), hit.ImpactPoint, FVector(5, 5, 5), FColor::Blue, false, 2.0f); // DEBUG -----------------------
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
		if (GetCurrentAmmo() != 0 && m_bCanShoot)
		{
			m_bIsFiring = true;

			// Decrement ammunition counter of current weapon
			InventoryComponent->ConsumeAmmo(m_currentWeapon);

			// Shoot raycast line
			RaycastShot();

			// Try and play the sound if specified
			PlayGunShotSFX();
		}

		// If the player has no ammunition 
		if (GetCurrentAmmo() == 0 && m_bCanShoot)
		{
			// If there is no reserve ammo stop shooting
				// Else start reloading
			GetTotalCurrentAmmo() == 0 ? StopFire() : StartReloadWeaponTimer();
		}
	}
}

void UTP_WeaponComponent::StartReloadWeaponTimer()
{
	if (ShouldPlayerReload() && m_bCanShoot)
	{
		// Stop the player from shooting
		m_bCanShoot = false;

		// Start reloading
		m_bIsReloading = true;

		// Force aim out of weapon
		AimOutSight();

		// Start reload timer
		Character->GetWorldTimerManager().SetTimer(m_handleReload, this, &UTP_WeaponComponent::ReloadWeapon, m_reloadTime, true);
	}
}

void UTP_WeaponComponent::ClearReloadWeaponTimer()
{
	// Clear reload timer
	Character->GetWorldTimerManager().ClearTimer(m_handleReload);
}

bool UTP_WeaponComponent::ShouldPlayerReload() const
{
	if (InventoryComponent->GetTotalAmmoCount(m_currentWeapon) != 0)
		// Has the player run out of ammunition 
		return InventoryComponent->GetAmmoCount(m_currentWeapon) < InventoryComponent->GetMaxAmmoInCatridgeCount(m_currentWeapon) ? true : false;

	return false;
}

void UTP_WeaponComponent::PlayGunShotSFX()
{
	if (m_gunArray[m_weaponIndex]->FireSound != nullptr)
		UGameplayStatics::PlaySoundAtLocation(this, m_gunArray[m_weaponIndex]->FireSound, Character->GetActorLocation());
}

void UTP_WeaponComponent::AimInSight()
{
	if (!m_bIsReloading)
	{
		m_bIsAimingIn = true;
		
		PlayADSTimeline();
	}
}

void UTP_WeaponComponent::AimOutSight()
{
	m_bIsAimingIn = false;

	ReverseADSTimeline();
}

void UTP_WeaponComponent::ReloadWeapon()
{
	// Calculate the ammunition counter of the weapon after reload
	InventoryComponent->ReloadWeapon(m_currentWeapon);
	
	// Clear the reload timer
	ClearReloadWeaponTimer();

	m_bIsReloading = false;

	// Allow the player to shoot again
	m_bCanShoot = true;
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

	StopFire();

	ClearReloadWeaponTimer();

	ShouldPlayerReload() ? StartReloadWeaponTimer() : m_bCanShoot = true;
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

bool UTP_WeaponComponent::GetIsReloading() const
{
	return m_bIsReloading;
}

bool UTP_WeaponComponent::GetIsAimingIn() const
{
	return m_bIsAimingIn;
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
	//
	// MESH ARMS IDLE LOCATION
	//(X=-1.949723,Y=0.000000,Z=-163.721813)

	