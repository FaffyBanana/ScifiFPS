// Copyright Epic Games, Inc. All Rights Reserved.

#include "ScifiFPSCharacter.h"
#include "ScifiFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/GameModeBase.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/TimelineComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AScifiFPSCharacter

AScifiFPSCharacter::AScifiFPSCharacter()
	:m_defaultWalkSpeed (500.0f)
	,m_defaultSprintSpeed (1000.0f)
{
	// Player can tick 
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	/* Create a CameraComponent */
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	const FVector fpsCameraLocation(-10.f, 0.f, 60.f);
	FirstPersonCameraComponent->SetRelativeLocation(fpsCameraLocation); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	/* Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn) */
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	const FVector mesh1PLocation(-30.f, 0.f, -150.f);
	Mesh1P->SetRelativeLocation(mesh1PLocation);

	/* Create a weapon placement component that weapons will attach to */
	WeaponPlacementComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPlacement"));
	WeaponPlacementComponent->SetupAttachment(FirstPersonCameraComponent);
	const FVector weaponPlacementLocation(20.0f, 20.0f, -20.0f);
	WeaponPlacementComponent->SetRelativeLocation(weaponPlacementLocation);

	/* Components */
	WeaponComponent = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("WeaponComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	/* Find the gameover menu widget */
	static ConstructorHelpers::FClassFinder<UUserWidget> GameOverWidgetClassFinder(TEXT("/Game/FirstPerson/UI/WBP_Gameover"));
	if (GameOverWidgetClassFinder.Class)
	{
		GameOverWidgetClass = GameOverWidgetClassFinder.Class;
	}

	// Set default walk speed
	GetCharacterMovement()->MaxWalkSpeed = m_defaultWalkSpeed;

	/*static ConstructorHelpers::FClassFinder<UUserWidget> playerHUDWidgetClassFinder(TEXT("/Game/FirstPerson/Blueprints/Widgets/WBP_PlayerHUD"));
	if (playerHUDWidgetClassFinder.Class)
	{
		PlayerHUDWidgetClass = playerHUDWidgetClassFinder.Class;
	}*/
}

void AScifiFPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	//if (PlayerHUDWidgetClass)
	//{
	//	PlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDWidgetClass);
	//	PlayerHUDWidget->AddToViewport(0);
	//}
}

//////////////////////////////////////////////////////////////////////////// Input

void AScifiFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AScifiFPSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AScifiFPSCharacter::Look);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AScifiFPSCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AScifiFPSCharacter::StopSprint);
	}
}

void AScifiFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*if (HealthComponent->GetHealth() <= 0)
	{
		if (GameOverWidgetClass)
		{
			GameOverWidget = CreateWidget<UUserWidget>(GetGameInstance(), GameOverWidgetClass);
			GameOverWidget->AddToViewport();
		}
	}*/
}

void AScifiFPSCharacter::Respawn()
{
	HealthComponent->ResetHealth();
	
	GetWorld()->GetAuthGameMode()->RestartPlayer(GetWorld()->GetFirstPlayerController());
}

void AScifiFPSCharacter::Move(const FInputActionValue& Value)
{
	// Input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AScifiFPSCharacter::Look(const FInputActionValue& Value)
{
	// Input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AScifiFPSCharacter::Sprint() 
{
	GetCharacterMovement()->MaxWalkSpeed = m_defaultSprintSpeed;
}

void AScifiFPSCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = m_defaultWalkSpeed;
}

void AScifiFPSCharacter::SetHasRifle(const bool bNewHasRifle)
{
	m_bHasRifle = bNewHasRifle;
}

bool AScifiFPSCharacter::GetHasRifle() const
{
	return m_bHasRifle;
}

UHealthComponent* AScifiFPSCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

UInventoryComponent* AScifiFPSCharacter::GetInventoryComponent() const
{
	return InventoryComponent;
}

UTP_WeaponComponent* AScifiFPSCharacter::GetWeaponComponent() const
{
	return WeaponComponent;
}

