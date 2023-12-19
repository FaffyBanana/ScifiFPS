// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveAlongPatrolRouteTask.h"
#include "AIInterface.h"
#include "EnemyAIManager.h"
#include "EnemyAIController.h"


EBTNodeResult::Type UMoveAlongPatrolRouteTask::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{

	AAIController* controllerAI = Cast<AAIController>(ownerComp.GetAIOwner());

	if (controllerAI)
	{
		AEnemyAIManager* enemyManager = Cast<AEnemyAIManager>(controllerAI->GetPawn());
		if (enemyManager)
		{
			APatrolRoute* patrolRoute = enemyManager->GetPatrolRoute();
			FVector splineWorldLocation = patrolRoute->GetSplinePointAtWorldPosition();
			controllerAI->MoveToLocation(splineWorldLocation, 10.0f);
			patrolRoute->IncrementPatrolRoute();
			return EBTNodeResult::Succeeded;
		}
	}

	//if(ownerComp.GetAIOwner()->Implements<UAIInterface>())
	//{
	//	IAIInterface* interfaceAI = Cast<IAIInterface>(ownerComp.GetAIOwner()); // Double check this works
	//	if(interfaceAI)
	//	{
	//		APatrolRoute* patrolRoute = interfaceAI->GetPatrolRoute();
	//		FVector splineWorldLocation = patrolRoute->GetSplinePointAtWorldPosition();
	//		ownerComp.GetAIOwner()->MoveToLocation(splineWorldLocation, 10.0f);

	//		patrolRoute->IncrementPatrolRoute();
	//		return EBTNodeResult::Succeeded;
	//	}
	//}
	return EBTNodeResult::Failed;
}