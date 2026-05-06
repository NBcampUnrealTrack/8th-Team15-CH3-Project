// BattleSystemComponent.cpp


#include "BattleSystemComponent.h"

UBattleSystemComponent::UBattleSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UBattleSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	HP = MaxHP;
}

void UBattleSystemComponent::DamageCalculation(float attackerATK)
{
	if (!bisDead)
	{
		HP = HP - attackerATK;

		if (HP <= 0)
		{
			HP = 0;
			bisDead = true;
			OnDeath.Broadcast();
		}
	}
}

void UBattleSystemComponent::Heal()
{
	HP = FMath::Min(HP + ATK * 0.45f, MaxHP);
}





