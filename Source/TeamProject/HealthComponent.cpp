// HealthComponent.cpp

#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	bIsDead = false;
}

void UHealthComponent::TakeDamage(float DamageAmount)
{
	if (bIsDead) return;

	CurrentHealth = FMath::Max(CurrentHealth - DamageAmount, 0.0f);

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		OnDeath.Broadcast();
	}
}

void UHealthComponent::Heal()
{
	if (bIsDead) return;

	CurrentHealth = FMath::Min(CurrentHealth + ATK * 0.45f, MaxHealth);
}




