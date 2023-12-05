// Fill out your copyright notice in the Description page of Project Settings.


#include "ClearFocusTask.h"

#include "EnemyAIController.h"


EBTNodeResult::Type UClearFocusTask::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	AEnemyAIController* controllerAI = Cast<AEnemyAIController>(ownerComp.GetAIOwner());

	if (controllerAI)
	{
		controllerAI->ClearFocus(EAIFocusPriority::Gameplay);
		return EBTNodeResult::Succeeded;

	}

	return EBTNodeResult::Failed;
}