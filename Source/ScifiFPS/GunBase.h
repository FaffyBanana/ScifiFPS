// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "GunBase.generated.h"


/*****************************************************************************************************
 * Type: Class
 *
 * Name: AGunBase
 *
 * Author: Jaber Ahmed
 *
 * Purpose: Base class for all guns
 *
 * References: N/A
 *
 * See Also: UTP_WeaponComponent
 *
 * Change Log:
 * Date          Initials    Version     Comments
 * 29/08/2023    JA          V1.0        N/A
*****************************************************************************************************/
UCLASS()
class SCIFIFPS_API AGunBase : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGunBase();

	UFUNCTION(BlueprintCallable)
	/* Returns the skeletal mesh of the gun */
	USkeletalMeshComponent* GetGunSkeletalMeshComponent() const;

public:
	///** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	/** Skeletal mesh of weapon */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* GunSkeletalMeshComponent;
};
