// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"
#include "Perception/AISense.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include <Perception/AISenseConfig_Sight.h>
#include <Perception/AISenseConfig_Hearing.h>
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SCIFIFPS_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();
	
	virtual void BeginPlay() override;
	
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const;

	//UFUNCTION()
	//void OnTargetPerceptionUpdated_Delegate(AActor* Actor, FAIStimulus Stimulus);

private:
	virtual void OnPossess(APawn* pawn) override;

	void OnUpdated();

	void StartEnemyTimer();
public:
	//UPROPERTY(VisibleAnywhere, Category = "AI")
	//TObjectPtr<UAIPerceptionComponent> PerceptionComp = nullptr; // Perception Component

	//UPROPERTY(EditAnywhere, Category = "AI")
	//TObjectPtr<class UAISenseConfig_Sight> SightConfig = nullptr; // Sight configuration

	//UPROPERTY(EditAnywhere, Category = "AI")
	//UAISenseConfig_Hearing* HearingConfig; // Hearing configuration

private:
	UBehaviorTreeComponent* BehaviourTreeComp;
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName m_enemyActor; // Target

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName m_isPlayerInCloseRange;

	//UPROPERTY(EditDefaultsOnly, Category = "AI")
	//FName m_hasLineOfSight; 

	//UPROPERTY(EditDefaultsOnly, Category = "AI")
	//FName m_patrolLocation; // Target


	//float m_sightRadius; // Sight radius
	//float m_loseSightRadius; // Lose sight radius
	//float m_peripheralVisionAngleDegrees; // Peripheral vision
	//float m_chaseSpeed; // Enemy run (chase) speed

	//FTimerHandle m_enemyTimer;
};
