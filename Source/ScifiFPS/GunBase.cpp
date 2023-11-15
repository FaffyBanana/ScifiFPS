// Fill out your copyright notice in the Description page of Project Settings.


#include "GunBase.h"

// Sets default values
AGunBase::AGunBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GunSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));
	ScopeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScopeMeshComponent"));
	ScopeMeshComponent->SetupAttachment(GunSkeletalMeshComponent);
	ScopeMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	SetRootComponent(GunSkeletalMeshComponent);
}

USkeletalMeshComponent* AGunBase::GetGunSkeletalMeshComponent() const
{
	return GunSkeletalMeshComponent;
}

// Called when the game starts or when spawned
void AGunBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

