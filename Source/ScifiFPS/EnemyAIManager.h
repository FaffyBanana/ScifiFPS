#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "TP_WeaponComponent.h"
#include "HealthComponent.h"
#include "Components/SphereComponent.h"
#include "AIInterface.h"
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
 * ???/09/2023	 JA			 v1.0		 Quick and simple AI system
 * 04/12/2023    JA          v2.0        Re-doing the AI system from the ground up
*****************************************************************************************************/
UCLASS()
class SCIFIFPS_API AEnemyAIManager : public ACharacter, public IAIInterface
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

	//void ShootPlayer();

	UHealthComponent* GetHealthComponent() const;

	//void Death();

	virtual void Attack() {}

	virtual void WieldWeapon() {}

	virtual void ChaseTarget();

	virtual APatrolRoute* GetPatrolRoute() override { return PatrolRoute; }

public:
	UBehaviorTree* GetBehaviourTree() const { return BehaviourTree; }

	/* Return Weapon Component */
	UTP_WeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

	UFUNCTION(BlueprintCallable)
	bool GetIsAttacking() const { return m_isAttacking; }

	/*UPROPERTY(EditAnywhere)
	int32 ShotPercentage;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
/*
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/

	UFUNCTION()
	void SetIsAttacking(const bool boolean);

protected:
	UPROPERTY(EditAnywhere, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviourTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	/** Weapon component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	UTP_WeaponComponent* WeaponComponent;

	UPROPERTY(EditAnywhere, Category = AI)
	APatrolRoute* PatrolRoute;

private:
	//bool ShotHitChance(const uint32 percentage) const; // Chance of bullet hitting player

	bool m_isAttacking;

private:
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereCollisionComponent;*/

	
	

};
