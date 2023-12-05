#pragma once

#include "CoreMinimal.h"
#include "EnemyAIManager.h"
#include "Animation/AnimMontage.h"
#include "MeleeEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SCIFIFPS_API AMeleeEnemy : public AEnemyAIManager
{
	GENERATED_BODY()

public:
	AMeleeEnemy();

	virtual void Attack() override;

	virtual void WieldWeapon() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MeleeAnimation;
};
