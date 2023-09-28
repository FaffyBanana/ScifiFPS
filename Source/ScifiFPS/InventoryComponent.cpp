// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	m_maxPrimaryAmmunition = 30;
	m_maxSecondaryAmmunition = 30;

	m_maxAmmunition.Add(EAmmunitionType::AE_Primary, m_maxPrimaryAmmunition);
	m_maxAmmunition.Add(EAmmunitionType::AE_Secondary, m_maxSecondaryAmmunition);

	m_ammunitionType.Add(EAmmunitionType::AE_Primary, m_maxPrimaryAmmunition);
	m_ammunitionType.Add(EAmmunitionType::AE_Secondary, m_maxSecondaryAmmunition);
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

void UInventoryComponent::ConsumeAmmo(EAmmunitionType ammo)
{
	m_ammunitionType[ammo]--;
}

uint32 UInventoryComponent::GetAmmoCount(EAmmunitionType ammo) const
{
	 return *m_ammunitionType.Find(ammo);
}

void UInventoryComponent::ReloadWeapon(EAmmunitionType ammo)
{
	m_ammunitionType[ammo] = m_maxAmmunition[ammo];
}


