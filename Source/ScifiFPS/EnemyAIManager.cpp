#include "EnemyAIManager.h"
#include "ScifiFPSCharacter.h"
#include "EnemyAIController.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AEnemyAIManager::AEnemyAIManager()
{
	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UpdateMaxWalkSpeed(120.0f);

	/*SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	SphereCollisionComponent->SetupAttachment(RootComponent);
	SphereCollisionComponent->SetSphereRadius(100.0f);*/

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	WeaponComponent = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("WeaponComponent"));
}

// Called when the game starts or when spawned
void AEnemyAIManager::BeginPlay()
{
	Super::BeginPlay();

	// Attach weapon to player
	/*if (GetWeaponComponent())
	{
		GetWeaponComponent()->Attach
	}*/

	//SphereCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAIManager::OnOverlapBegin);
	//SphereCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemyAIManager::OnOverlapEnd);
}


// Called every frame
void AEnemyAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (HealthComponent->GetHealth() <= 0)
	{
		Death();
	}*/
}

// Called to bind functionality to input
void AEnemyAIManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyAIManager::UpdateMaxWalkSpeed(const float speed)
{
	GetCharacterMovement()->MaxWalkSpeed = speed;
}

//void AEnemyAIManager::ShootPlayer()
//{
//	if (ShotHitChance(ShotPercentage))
//	{
//		AScifiFPSCharacter* player = Cast<AScifiFPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
//		if (player)
//		{
//			player->GetHealthComponent()->TakeDamage();
//		}
//	}
//	
//}

UHealthComponent* AEnemyAIManager::GetHealthComponent() const
{
	return HealthComponent;
}
//
//void AEnemyAIManager::Death()
//{
//	Destroy();
//}
//
//void AEnemyAIManager::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	AScifiFPSCharacter* player = Cast<AScifiFPSCharacter>(OtherActor);
//	if (player)
//	{
//		AEnemyAIController* controllerAI = Cast<AEnemyAIController>(GetController());
//		{
//			if (controllerAI)
//			{
//				controllerAI->SetPlayerInCloseRange(true);
//				controllerAI->SetPlayerActor(player);
//				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, TEXT("Player in close proximity"));
//			}
//		}
//	}
//}

//void AEnemyAIManager::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	AScifiFPSCharacter* player = Cast<AScifiFPSCharacter>(OtherActor);
//	if (player)
//	{
//		AEnemyAIController* controllerAI = Cast<AEnemyAIController>(GetController());
//		{
//			if (controllerAI)
//			{
//				controllerAI->SetPlayerInCloseRange(false);
//				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Player has left proximity"));
//			}
//		}
//	}
//}