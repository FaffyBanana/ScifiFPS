// Fill out your copyright notice in the Description page of Project Settings.


#include "WieldWeaponTask.h"
#include "EnemyAIManager.h"
#include "EnemyAIController.h"


EBTNodeResult::Type UWieldWeaponTask::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	AAIController* controllerAI = Cast<AAIController>(ownerComp.GetAIOwner());

	if (controllerAI)
	{
		AEnemyAIManager* enemyManager = Cast<AEnemyAIManager>(controllerAI->GetPawn());
		if (enemyManager)
		{
			enemyManager->WieldWeapon();
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}