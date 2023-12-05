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

	m_playerActor = "PlayerActor";
	//m_isPlayerInCloseRange = "IsPlayerInCloseRange";
	m_hasLineOfSight = "HasLineOfSight";
	m_patrolLocation = "PatrolLocation";
	m_isWieldingWeapon = "IsWieldingWeapon";

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


//void AEnemyAIController::SetPlayerInCloseRange(const bool boolean)
//{
//	if (!BlackboardComp)
//	{
//		return;
//	}
//
//	BlackboardComp->SetValueAsBool(m_isPlayerInCloseRange, boolean);
//}

void AEnemyAIController::SetPlayerActor(AActor* PlayerActor)
{
	BlackboardComp->SetValueAsObject(m_playerActor, PlayerActor);

	
}

void AEnemyAIController::OnTargetPerceptionUpdated_Delegate(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor->ActorHasTag("Player") && Stimulus.WasSuccessfullySensed())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Player Detected"));
		GetWorldTimerManager().ClearTimer(m_enemyTimer);
		BlackboardComp->SetValueAsBool(m_hasLineOfSight, true);
		BlackboardComp->SetValueAsObject(m_playerActor, Actor);
	}
	else
	{
		float m_lineOfSightTimer = 4.0f;
		GetWorldTimerManager().SetTimer(m_enemyTimer, this, &AEnemyAIController::StartEnemyTimer, m_lineOfSightTimer);
	}
}

void AEnemyAIController::SetFocusOnTarget()
{
	AActor* actor = Cast<AActor>(BlackboardComp->GetValueAsObject(m_playerActor));
	if(actor)
	{
		SetFocus(actor);
	}
}

void AEnemyAIController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	const AEnemyAIManager* enemyManager = Cast<AEnemyAIManager>(pawn);

	if (enemyManager)
	{
		UBehaviorTree* const behaviourTree = enemyManager->GetBehaviourTree();
		if (behaviourTree)
		{
			BlackboardComp->InitializeBlackboard(*(behaviourTree->BlackboardAsset));
			BehaviourTreeComp->StartTree(*behaviourTree);
		}
	}
}

void AEnemyAIController::OnUpdated()
{
	AScifiFPSCharacter* player = Cast<AScifiFPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, SightConfig->GetSenseImplementation(), this);
	}
}

void AEnemyAIController::StartEnemyTimer()
{
	BlackboardComp->SetValueAsBool(m_hasLineOfSight, false);
	BlackboardComp->SetValueAsObject(m_playerActor, false);

}

