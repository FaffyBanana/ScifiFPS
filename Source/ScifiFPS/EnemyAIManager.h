#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "TP_WeaponComponent.h"
#include "HealthComponent.h"
#include "Components/SphereComponent.h"
#include "EnemyAIManager.generated.h"

/*****************************************************************************************************
 * Type: Class
 *
 * Name:
 *
 * Author: Jaber Ahmed
 *
 * Purpose:
 *
 * References: N/A
 *
 * See Also: N/A
 *
 * Change Log:
 * Date          Initials    Version     Comments
 * ----------    JA          -------     ----------------------------------------------
*****************************************************************************************************/
UCLASS()
class SCIFIFPS_API AEnemyAIManager : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyAIManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Update how fast the enemy can walk/run
	void UpdateMaxWalkSpeed(const float speed);

	void ShootPlayer();

	UHealthComponent* GetHealthComponent() const;

	void Death();

public:
	UBehaviorTree* GetBehaviourTree() const { return BehaviourTree; }

	UPROPERTY(EditAnywhere)
	int32 ShotPercentage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviourTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;
private:
	bool ShotHitChance(const uint32 percentage) const; // Chance of bullet hitting player


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereCollisionComponent;

	

};
