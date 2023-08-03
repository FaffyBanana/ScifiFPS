// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootPlayerTask.h"
#include "EnemyAIController.h"
#include "EnemyAIManager.h"

EBTNodeResult::Type UShootPlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyAIController* enemyController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

    if (enemyController)
    {
        AEnemyAIManager* enemyManager = Cast<AEnemyAIManager>(enemyController->GetPawn());
        if (enemyManager)
        {
            enemyManager->ShootPlayer();
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}