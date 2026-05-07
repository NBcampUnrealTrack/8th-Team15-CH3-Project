// HealthComponent.cpp

#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	HP = MaxHP;
	bIsDead = false;
}

void UHealthComponent::TakeDamage(float AttackerATK)
{
	if (bIsDead)
	{
		return;
	}

	HP = FMath::Max(HP - AttackerATK, 0.0f);
	OnHPChanged.Broadcast(HP);

	if (HP <= 0.0f)
	{
		bIsDead = true;
		OnDeath.Broadcast();
	}
}

void UHealthComponent::Heal()
{
	if (bIsDead)
	{
		return;
	}

	HP = FMath::Min(HP + ATK * 0.45f, MaxHP);
	OnHPChanged.Broadcast(HP);
}

bool UHealthComponent::IsDead() const
{
	return bIsDead;
}

float UHealthComponent::GetHP() const
{
	return HP;
}

float UHealthComponent::GetMaxHP() const
{
	return MaxHP;
}

float UHealthComponent::GetATK() const
{
	return ATK;
}




