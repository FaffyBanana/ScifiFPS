// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "SingleShotRifle.generated.h"

/*****************************************************************************************************
 * Type: Class
 *
 * Name: ASingleShotRifle
 *
 * Author: Jaber Ahmed
 *
 * Purpose: Singleshot gun, derived from the base class Gun Base
 *
 * References: N/A
 *
 * See Also: AGunBase, UTP_WeaponComponent
 *
 * Change Log:
 * Date          Initials    Version     Comments
 * 29/09/2023    JA          V1.0        N/A
*****************************************************************************************************/
UCLASS()
class SCIFIFPS_API ASingleShotRifle : public AGunBase
{
	GENERATED_BODY()

public:
	ASingleShotRifle();
};
