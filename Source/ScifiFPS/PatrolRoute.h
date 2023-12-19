// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PatrolRoute.generated.h"

UCLASS()
class SCIFIFPS_API APatrolRoute : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolRoute();

	void IncrementPatrolRoute();

	FVector GetSplinePointAtWorldPosition() const;

	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PatrolRoute, meta = (AllowPrivateAccess = "true"))
	USplineComponent* PatrolRoute;

	int m_patrolIndex;

	UPROPERTY(EditAnywhere, Category = PatrolRoute, meta = (ClampMin = "-1", ClampMax = "1"))
	int m_direction;
	

};
