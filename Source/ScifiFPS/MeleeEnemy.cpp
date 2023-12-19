#include "MeleeEnemy.h"

AMeleeEnemy::AMeleeEnemy()
{
}

void AMeleeEnemy::Attack()
{
	SetIsAttacking(true);
	if (MeleeAnimation != nullptr)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (animInstance != nullptr)
		{
			animInstance->Montage_Play(MeleeAnimation, 1.5f);
		}
	}
}

void AMeleeEnemy::WieldWeapon()
{
	
}

APatrolRoute* AMeleeEnemy::GetPatrolRoute()
{
	return PatrolRoute;
}
