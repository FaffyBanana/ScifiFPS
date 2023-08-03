// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasePlayerTask.h"
#include "EnemyAIController.h"
#include "EnemyAIManager.h"
EBTNodeResult::Type UChasePlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyAIController* enemyController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

    if(enemyController)
    {
        AEnemyAIManager* enemyManager = Cast<AEnemyAIManager>(enemyController->GetPawn());
        if(enemyManager)
        {
            enemyManager->UpdateMaxWalkSpeed(600.0f);
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;

}