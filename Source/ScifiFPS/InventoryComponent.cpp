// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent() 
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Set max ammunition of each weapon
	m_maxAmmunitionInCatridge.Add(EAmmunitionType::AE_Primary, m_ammunitionSettings.DefaultMaxPrimaryAmmunition);
	m_maxAmmunitionInCatridge.Add(EAmmunitionType::AE_Secondary, m_ammunitionSettings.DefaultMaxSecondaryAmmunition);

	// Set ammunition count of each weapon
	m_ammunitionCount.Add(EAmmunitionType::AE_Primary, m_maxAmmunitionInCatridge[EAmmunitionType::AE_Primary]);
	m_ammunitionCount.Add(EAmmunitionType::AE_Secondary, m_maxAmmunitionInCatridge[EAmmunitionType::AE_Secondary]);

	// Set ammunition count of each weapon
	m_totalAmmunitionCount.Add(EAmmunitionType::AE_Primary, m_ammunitionSettings.DefaultTotalPrimaryAmmunition);
	m_totalAmmunitionCount.Add(EAmmunitionType::AE_Secondary, m_ammunitionSettings.DefaultTotalSecondaryAmmunition);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::ReloadWeapon(EAmmunitionType ammo)
{
	/* If there is enough reserve ammunition to refill a full magazine */
	if (m_totalAmmunitionCount[ammo] >= m_maxAmmunitionInCatridge[ammo])
	{
		m_totalAmmunitionCount[ammo] -= (m_maxAmmunitionInCatridge[ammo] - m_ammunitionCount[ammo]);
		m_ammunitionCount[ammo] = m_maxAmmunitionInCatridge[ammo];
	}

	/* If there isn't enough reserve ammunition to refill a full magazine */
	else if (m_totalAmmunitionCount[ammo] < m_maxAmmunitionInCatridge[ammo] && m_totalAmmunitionCount[ammo] > 0)
	{
		/* If there isn't enough reserve for a full clip */
		if (m_totalAmmunitionCount[ammo] < (m_maxAmmunitionInCatridge[ammo] - m_ammunitionCount[ammo]))
		{
			m_ammunitionCount[ammo] += m_totalAmmunitionCount[ammo];
			m_totalAmmunitionCount[ammo] = 0;
		}
		/* If there is enough reserve for a full clip */
		else
		{
			m_totalAmmunitionCount[ammo] -= (m_maxAmmunitionInCatridge[ammo] - m_ammunitionCount[ammo]);
			m_ammunitionCount[ammo] = m_maxAmmunitionInCatridge[ammo];
		}
	}

	/* If the total ammunition goes below zero */
	else
	{
		m_totalAmmunitionCount[ammo] = 0;
	}
}

void UInventoryComponent::ConsumeAmmo(EAmmunitionType ammo)
{
	m_ammunitionCount[ammo]--;
}

int32 UInventoryComponent::GetAmmoCount(EAmmunitionType ammo) const
{
	 return m_ammunitionCount[ammo];
}

int32 UInventoryComponent::GetTotalAmmoCount(EAmmunitionType ammo) const
{
	return m_totalAmmunitionCount[ammo];
}

int32 UInventoryComponent::GetMaxAmmoInCatridgeCount(EAmmunitionType ammo) const
{
	return m_maxAmmunitionInCatridge[ammo];
}



