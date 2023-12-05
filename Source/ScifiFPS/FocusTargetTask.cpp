// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusTargetTask.h"
#include "EnemyAIController.h"


EBTNodeResult::Type UFocusTargetTask::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	AEnemyAIController* controllerAI = Cast<AEnemyAIController>(ownerComp.GetAIOwner());

	if (controllerAI)
	{
		controllerAI->SetFocusOnTarget();
		return EBTNodeResult::Succeeded;

	}

	return EBTNodeResult::Failed;
}