#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "TP_WeaponComponent.h"
#include "HealthComponent.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool ShotHitChance(const uint32 percentage) const; // Chance of shot being hit

public:
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
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviourTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere)
	uint32 ShotPercentage;
};
