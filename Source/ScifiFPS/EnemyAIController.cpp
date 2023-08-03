// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "EnemyAIManager.h"
#include "ScifiFPSCharacter.h"
#include <Kismet/GameplayStatics.h>

AEnemyAIController::AEnemyAIController()
	:m_sightRadius(3000.0f)
	,m_loseSightRadius(1500.0f)
	,m_peripheralVisionAngleDegrees(35.0f)
{
	BehaviourTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourTreeComponent"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	m_enemyActor = "EnemyActor";
	m_hasLineOfSight = "HasLineOfSight";
	m_patrolLocation = "PatrolLocation";

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	if (PerceptionComp)
	{
		// Perception config
		PerceptionComp->ConfigureSense(*SightConfig);
		PerceptionComp->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	}

	if (SightConfig)
	{
		// Sight config
		SightConfig->SightRadius = m_sightRadius;
		SightConfig->LoseSightRadius = m_loseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = m_peripheralVisionAngleDegrees;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated_Delegate);
}

UBlackboardComponent* AEnemyAIController::GetBlackboardComp() const
{
	return BlackboardComp;
}

void AEnemyAIController::OnTargetPerceptionUpdated_Delegate(AActor* Actor, FAIStimulus Stimulus)
{
	if(Actor->ActorHasTag("Player") && Stimulus.WasSuccessfullySensed())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Player Detected"));
		GetWorldTimerManager().ClearTimer(m_enemyTimer);
		BlackboardComp->SetValueAsBool(m_hasLineOfSight, true);
		BlackboardComp->SetValueAsObject(m_enemyActor, Actor);
	}
	else
	{
		float m_lineOfSightTimer = 4.0f;
		GetWorldTimerManager().SetTimer(m_enemyTimer, this, &AEnemyAIController::StartEnemyTimer, m_lineOfSightTimer);
	}
}

void AEnemyAIController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	AEnemyAIManager* enemyManager = Cast<AEnemyAIManager>(pawn);

	if(enemyManager)
	{
		if(enemyManager->BehaviourTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*(enemyManager->BehaviourTree->BlackboardAsset));
		}
		BehaviourTreeComp->StartTree(*enemyManager->BehaviourTree);
	}
}

void AEnemyAIController::OnUpdated()
{
	AScifiFPSCharacter* player = Cast<AScifiFPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if(player)
	{
		UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, SightConfig->GetSenseImplementation(), this);
	}
}

void AEnemyAIController::StartEnemyTimer()
{
	BlackboardComp->SetValueAsBool(m_hasLineOfSight, false);
	BlackboardComp->SetValueAsObject(m_enemyActor, false);

}

