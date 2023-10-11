// Copyright Epic Games, Inc. All Rights Reserved.

#include "ScifiFPSCharacter.h"
#include "ScifiFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/GameModeBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AScifiFPSCharacter

AScifiFPSCharacter::AScifiFPSCharacter()
{

	PrimaryActorTick.bCanEverTick = true;

	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Create a weapon component 
	WeaponComponent = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("Weapon"));

	// Create health component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// Create inventory component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// Find BP widget 
	static ConstructorHelpers::FClassFinder<UUserWidget> GameOverWidgetClassFinder(TEXT("/Game/FirstPerson/UI/WBP_Gameover"));
	if (GameOverWidgetClassFinder.Class)
	{
		GameOverWidgetClass = GameOverWidgetClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> playerHUDWidgetClassFinder(TEXT("/Game/FirstPerson/Blueprints/Widgets/WBP_PlayerHUD"));
	if (playerHUDWidgetClassFinder.Class)
	{
		PlayerHUDWidgetClass = playerHUDWidgetClassFinder.Class;
	}

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

	// Attach weapon to player
	if (WeaponComponent)
	{
		WeaponComponent->AttachWeapon(this);
	}

	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDWidgetClass);
		PlayerHUDWidget->AddToViewport(0);
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AScifiFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AScifiFPSCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AScifiFPSCharacter::Look);
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
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AScifiFPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AScifiFPSCharacter::SetHasRifle(const bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AScifiFPSCharacter::GetHasRifle() const
{
	return bHasRifle;
}

UHealthComponent* AScifiFPSCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

UTP_WeaponComponent* AScifiFPSCharacter::GetWeaponComponent() const
{
	return WeaponComponent;
}
