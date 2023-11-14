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
	:ShootingDistance (2000.0f) 
	,TimeBetweenShots (0.15f)
	,m_weaponIndex (0)
	,m_currentWeapon (EAmmunitionType::AE_Primary)
	,m_bCanShoot (true)
	,m_bIsFiring (false)
	,m_bIsReloading (false)
	,m_bIsAimingIn (false)
	,m_reloadTime (2.25f)
	,m_aDSFOV (75.0f)
	,m_aDSDistanceToCamera (10.0f)
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
	
	/* Get a reference to the player who owns this component */
	Character = Cast<AScifiFPSCharacter>(GetOwner());
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		InventoryComponent = Character->GetInventoryComponent();
		InitWeapons();
		InitADSTimeline();
	}
}

void UTP_WeaponComponent::AttachWeapon()
{
	/* Setup weapon attachment */
	const FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, true);
	for(int i = 0; i < m_weaponArray.Num(); i++)
	{
		m_weaponArray[i]->GetGunSkeletalMeshComponent()->
			AttachToComponent(Character->GetMesh1P(), attachmentRules, FName(TEXT("GripPoint")));
	}

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

	//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.0f); // DEBUG -----------------------

	// If line trace has hit an object
	if (bHit)
	{
		/* If AI has been hit */
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
		if (GetCurrentAmmoOfCurrentWeapon() != 0 && m_bCanShoot)
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
		if (GetCurrentAmmoOfCurrentWeapon() == 0 && m_bCanShoot)
		{
			ShouldPlayerReload() ? StartReloadWeaponTimer() : StopFire();
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
	if (GetReserveAmmoOfCurrentWeapon() != 0)
		return GetCurrentAmmoOfCurrentWeapon() < GetMaxAmmoCatridgeOfCurrentWeapon() ? true : false;

	return false;
}

void UTP_WeaponComponent::PlayGunShotSFX()
{
	if (m_weaponArray[m_weaponIndex]->FireSound != nullptr)
		UGameplayStatics::PlaySoundAtLocation(this, m_weaponArray[m_weaponIndex]->FireSound, Character->GetActorLocation());
}

void UTP_WeaponComponent::InitWeapons()
{
	/* Spawn guns */
	const FTransform weaponPlacementTransform = Character->GetWeaponPlacementComponent()->GetComponentTransform();
	const FActorSpawnParameters spawnInfo;
	PrimaryWeapon = SpawnWeapon(PrimaryWeaponRef, weaponPlacementTransform, spawnInfo);
	SecondaryWeapon = SpawnWeapon(SecondaryWeaponRef, weaponPlacementTransform, spawnInfo);

	/* Set primary gun defaults */
	if (PrimaryWeapon)
	{
		m_weaponArray.Add(PrimaryWeapon);
		m_isWeaponActiveMap.Add(EAmmunitionType::AE_Primary, false);
		m_isAutomaticMap.Add(EAmmunitionType::AE_Primary, true);
	}

	/* Set secondary gun defaults */
	if (SecondaryWeapon)
	{
		m_weaponArray.Add(SecondaryWeapon);
		m_isWeaponActiveMap.Add(EAmmunitionType::AE_Secondary, false);
		m_isAutomaticMap.Add(EAmmunitionType::AE_Secondary, false);
	}

	AttachWeapon();

	if (!m_weaponArray.IsEmpty())
	{
		/* Set current weapon as active */
		m_isWeaponActiveMap[m_currentWeapon] = true;
	}
}

void UTP_WeaponComponent::InitADSTimeline()
{
	/* ADS Timeline */
	if (ADSCurveFloat)
	{
		/* Create ADS curve timeline */
		ADSCurveTimeline = NewObject<UTimelineComponent>(this, FName("ADSTimelineAnimation"));
		ADSCurveTimeline->CreationMethod = EComponentCreationMethod::SimpleConstructionScript;
		Character->BlueprintCreatedComponents.Add(ADSCurveTimeline);

		/* Bind the ADS function to the timeline */
		FOnTimelineFloat onTimelineCallback;
		onTimelineCallback.BindUFunction(this, FName(TEXT("AimInTimelineProgress")));
		ADSCurveTimeline->AddInterpFloat(ADSCurveFloat, onTimelineCallback);
		ADSCurveTimeline->SetLooping(false);
		ADSCurveTimeline->RegisterComponent();

		/* ADS variable defaults */
		m_weaponPlacementLocation = Character->GetWeaponPlacementComponent()->GetRelativeLocation();
		m_meshPlacementLocation = Character->GetMesh1P()->GetRelativeLocation();
		m_fOV = Character->GetFirstPersonCameraComponent()->FieldOfView;
	}
}

AGunBase* UTP_WeaponComponent::SpawnWeapon(const TSubclassOf<AGunBase> weaponRef, const FTransform transform, const FActorSpawnParameters spawnInfo)
{
	/* Spawn all guns */
	return GetWorld()->SpawnActor<AGunBase>(weaponRef, transform, spawnInfo);
}

AGunBase* UTP_WeaponComponent::SpawnWeapon(const TSubclassOf<AGunBase> weaponRef, const FVector location, const FRotator rotator, const FActorSpawnParameters spawnInfo)
{
	/* Spawn all guns */
	return GetWorld()->SpawnActor<AGunBase>(weaponRef, location, rotator, spawnInfo);
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

void UTP_WeaponComponent::AimInTimelineProgress(float value)
{
	// Get location of the aim socket on the gun
	FVector aimSocketLocation = m_weaponArray[m_weaponIndex]->GetGunSkeletalMeshComponent()
		->GetSocketTransform(FName(TEXT("ADS_Socket")), ERelativeTransformSpace::RTS_Actor).GetLocation();

	// Create a vector for the camera ADS
	FVector cameraADSlerp(m_aDSDistanceToCamera, 0.0f, aimSocketLocation.Y);

	// Lerp the original weapon placement with the aim in location using the timeline's alpha value
	FVector lerpADSLocation = UKismetMathLibrary::VLerp(m_weaponPlacementLocation, cameraADSlerp, value);
	Character->GetWeaponPlacementComponent()->SetRelativeLocation(lerpADSLocation);

	//FVector meshADSLocation(-16.000000, 1.450000, -165.700000); // For primary weapon

	// Lerp the ADS FOV and the Non ADS FOV using the timeline's alpha value 
	float tempADSFOV = UKismetMathLibrary::Lerp(90, m_aDSFOV, value);
	Character->GetFirstPersonCameraComponent()->SetFieldOfView(tempADSFOV);

	// Move the mesh arms into its ADS location and back
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

void UTP_WeaponComponent::ReloadWeapon()
{
	// Calculate the ammunition counter of the weapon after reload
	InventoryComponent->ReloadWeapon(m_currentWeapon);
	
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
	if (m_weaponArray.IsValidIndex(tempWeaponIndex))
	{
		m_weaponIndex = tempWeaponIndex;
	}

	// Otherwise go to the top or the bottom of the array depedning on the direction of the mouse scroll
	else
	{
		m_weaponIndex < 0 ? m_weaponIndex = m_weaponArray.Num() - 1 : m_weaponIndex = 0;
	}

	SwitchToNextWeapon();
	StopFire();
	ClearReloadWeaponTimer();
	GetReserveAmmoOfCurrentWeapon() == 0 ? StartReloadWeaponTimer() : m_bCanShoot = true;
}

void UTP_WeaponComponent::SwitchToNextWeapon()
{
	/* Set gun actors as invisible */
	for (int i = 0; i < m_weaponArray.Num(); i++)
	{
		m_weaponArray[i]->GetGunSkeletalMeshComponent()->SetVisibility(false);
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
	m_weaponArray[m_weaponIndex]->GetGunSkeletalMeshComponent()->SetVisibility(true);
	m_isWeaponActiveMap[m_currentWeapon] = true;

}
int32 UTP_WeaponComponent::GetCurrentAmmoOfCurrentWeapon() const
{
	return InventoryComponent->GetCurrentAmmoCount(m_currentWeapon);
}

int32 UTP_WeaponComponent::GetReserveAmmoOfCurrentWeapon() const
{
	return InventoryComponent->GetReserveAmmoCount(m_currentWeapon);
}

int32 UTP_WeaponComponent::GetMaxAmmoCatridgeOfCurrentWeapon() const
{
	return InventoryComponent->GetMaxAmmoInCatridgeCount(m_currentWeapon);
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

	