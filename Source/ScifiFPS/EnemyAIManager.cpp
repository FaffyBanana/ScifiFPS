#include "EnemyAIManager.h"

// Sets default values
AEnemyAIManager::AEnemyAIManager()
{
	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UpdateMaxWalkSpeed(120.0f);
}

// Called when the game starts or when spawned
void AEnemyAIManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemyAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyAIManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyAIManager::UpdateMaxWalkSpeed(float speed)
{
	GetCharacterMovement()->MaxWalkSpeed = speed;
}
