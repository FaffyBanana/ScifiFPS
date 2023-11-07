// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_maxHealth = 100;
	m_health = m_maxHealth;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

int32 UHealthComponent::GetHealth()const
{
	return m_health;
}

int32 UHealthComponent::GetMaxHealth() const
{
	return m_maxHealth;
}

void UHealthComponent::TakeDamage()
{
	if(m_health > 0)
	{
		m_health -= 20;
	}

}

void UHealthComponent::ResetHealth()
{
	m_health = m_maxHealth;
}

UHealthComponent* UHealthComponent::GetHealthComponent() const
{
	return nullptr;
}

