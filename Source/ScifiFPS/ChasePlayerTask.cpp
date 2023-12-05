// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasePlayerTask.h"
#include "EnemyAIController.h"
#include "EnemyAIManager.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"

EBTNodeResult::Type UChasePlayerTask::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
    AEnemyAIController* enemyController = Cast<AEnemyAIController>(ownerComp.GetAIOwner());
    if (enemyController)
    {
        AEnemyAIManager* enemyManager = Cast<AEnemyAIManager>(enemyController->GetPawn());
        if (enemyManager)
        {
            enemyManager->UpdateMaxWalkSpeed(600.0f);
            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}
