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
	IsAutomatic = true;
	AssaultRifleActive = true;

	m_weaponIndex = 0;

	PrimaryGun = CreateDefaultSubobject<UChildActorComponent>(TEXT("PrimaryGun"));
	SecondaryGun = CreateDefaultSubobject<UChildActorComponent>(TEXT("SecondaryGun"));

	GunArray.Add(PrimaryGun);
	GunArray.Add(SecondaryGun);
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryComponent = GetOwner()->FindComponentByClass<UInventoryComponent>();
}

void UTP_WeaponComponent::AttachWeapon(AScifiFPSCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	//PrimaryWeapon->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	//SecondaryWeapon->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	/* Maybe attach child actor componnents here ------------- */

	//SwitchWeapons(0);

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

	if (InventoryComponent)
	{
		if (AssaultRifleActive)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(InventoryComponent->GetAssaultRifleAmmo()));
			InventoryComponent->ConsumeAssaultRifleAmmo();
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
	int tempValue = UKismetMathLibrary::FTrunc(index);
	int tempWeaponIndex = tempValue = m_weaponIndex;

}

void UTP_WeaponComponent::SwitchToNextWeapon()
{

	//// Set gun actors as invisible
	//for (int i = 0; i < GunArray.Num(); i++)
	//{

	//	GunArray[i]->SetVisibility(false);
	//}

	//// Set current weapon to be visible
	//GunArray[m_weaponIndex]->SetVisibility(true);

	//// Set attachment
	//FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	//GunArray[m_weaponIndex]->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	switch (m_weaponIndex)
	{
	/* Primary Weapon */
	case 0:
		if (GunArray.Num() > 1)
		{
			m_weaponIndex = 1;
			SwitchWeapons(m_weaponIndex);
		}
		break;
	
	/* Secondary Weapon*/
	case 1:
		if (GunArray.Num() > 2)
		{
			m_weaponIndex = 2;
			SwitchWeapons(m_weaponIndex);
		}
		else
		{
			m_weaponIndex = 0;
			SwitchWeapons(m_weaponIndex);
		}
		break;

	/* Tertiary Weapon */
	case 2:
		if (GunArray.Num() > 3)
		{
			m_weaponIndex = 3;
			SwitchWeapons(m_weaponIndex);
		}
		else
		{
			m_weaponIndex = 0;
			SwitchWeapons(m_weaponIndex);
		}
		break;

	default:
		break;
	}
}

/* WeaponChange(index)
* int tempIndex = index +1;
	if(GunArray.Num() > tempIndex)
		m_weaponIndex = tempIndex;
		SwitchWeapons(m_weaponIndex);
	*/




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

	//IsAutomatic ? GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Automatic On")) : GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Automatic Off"));
	
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

	