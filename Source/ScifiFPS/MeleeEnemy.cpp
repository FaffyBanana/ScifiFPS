#include "MeleeEnemy.h"

AMeleeEnemy::AMeleeEnemy()
{
}

void AMeleeEnemy::Attack()
{
	if (MeleeAnimation != nullptr)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (animInstance != nullptr)
		{
			animInstance->Montage_Play(MeleeAnimation, 1.0f);
		}
	}
}

void AMeleeEnemy::WieldWeapon()
{
	
}
