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

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	// Default line trace distance
	ShootingDistance = 2000.0f;

	// Set time between shots
	TimeBetweenShots = 0.15f;
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

void UTP_WeaponComponent::AttachWeapon(AScifiFPSCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
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
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::StartFire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::StopFire);

		}
	}
}

void UTP_WeaponComponent::StartFire()
{
	Fire();
	Character->GetWorldTimerManager().SetTimer(m_handleRefire, this, &UTP_WeaponComponent::Fire, TimeBetweenShots, true);
}

void UTP_WeaponComponent::StopFire()
{
	Character->GetWorldTimerManager().ClearTimer(m_handleRefire);
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

	