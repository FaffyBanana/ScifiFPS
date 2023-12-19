// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolRoute.h"

// Sets default values
APatrolRoute::APatrolRoute()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_patrolIndex = 0;
	PatrolRoute = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolRoute"));
}

void APatrolRoute::IncrementPatrolRoute()
{
	m_patrolIndex += m_direction;

	if (m_patrolIndex == PatrolRoute->GetNumberOfSplinePoints() - 1)
	{
		m_direction = -1;
	}

	else if (m_patrolIndex == 0)
	{
		m_direction = 1;
	}
}

FVector APatrolRoute::GetSplinePointAtWorldPosition() const
{
	return PatrolRoute->GetLocationAtSplinePoint(m_patrolIndex, ESplineCoordinateSpace::World);
}



