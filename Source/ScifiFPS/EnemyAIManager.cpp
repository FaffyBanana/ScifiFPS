#include "EnemyAIManager.h"
#include "ScifiFPSCharacter.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AEnemyAIManager::AEnemyAIManager()
{
	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UpdateMaxWalkSpeed(120.0f);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
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

	if (HealthComponent->GetHealth() <= 0)
	{
		Death();
	}
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

void AEnemyAIManager::ShootPlayer()
{
	AScifiFPSCharacter* player = Cast<AScifiFPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if(player)
	{
		player->GetHealthComponent()->TakeDamage();
	}
}

UHealthComponent* AEnemyAIManager::GetHealthComponent()
{
	return HealthComponent;
}

void AEnemyAIManager::Death()
{
	Destroy();
}
