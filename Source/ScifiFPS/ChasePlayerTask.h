// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ChasePlayerTask.generated.h"

/**
 * 
 */
UCLASS()
class SCIFIFPS_API UChasePlayerTask : public UBTTaskNode
{
	GENERATED_BODY()

private:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) override;
};
