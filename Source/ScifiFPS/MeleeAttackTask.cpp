// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttackTask.h"
#include "EnemyAIManager.h"
#include "AIController.h"

EBTNodeResult::Type UMeleeAttackTask::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) 
{
	AAIController* controllerAI = Cast<AAIController>(ownerComp.GetAIOwner());

	if (controllerAI)
	{
		AEnemyAIManager* enemyManager = Cast<AEnemyAIManager>(controllerAI->GetPawn());
		if (enemyManager)
		{
			enemyManager->Attack();
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
