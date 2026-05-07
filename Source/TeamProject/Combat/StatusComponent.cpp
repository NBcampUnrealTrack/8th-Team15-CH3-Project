// StatusComponent.cpp


#include "Combat/StatusComponent.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();
	HP = MaxHP;
	bIsDead = false;
}

void UStatusComponent::ReceiveDamage(float AttackerATK)
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

void UStatusComponent::Heal()
{
	if (bIsDead)
	{
		return;
	}

	HP = FMath::Min(HP + ATK * 0.45f, MaxHP);
	OnHPChanged.Broadcast(HP);
}

bool UStatusComponent::IsDead() const
{
	return bIsDead;
}

float UStatusComponent::GetHP() const
{
	return HP;
}

float UStatusComponent::GetMaxHP() const
{
	return MaxHP;
}

float UStatusComponent::GetATK() const
{
	return ATK;
}

void UStatusComponent::SetHP(float NewHP)
{
	HP = NewHP;

	return;
}

void UStatusComponent::SetATK(float NewATK)
{
	ATK = NewATK;

	return;
}