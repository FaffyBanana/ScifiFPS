// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int UInventoryComponent::GetAssaultRifleAmmo()
{
	return m_assaultRifleAmmoCount;
}

void UInventoryComponent::ConsumeAssaultRifleAmmo()
{
	m_assaultRifleAmmoCount--;
}

void UInventoryComponent::SetAssaultRifleAmmo(int ammoCount)
{
	m_assaultRifleAmmoCount = ammoCount;
}

