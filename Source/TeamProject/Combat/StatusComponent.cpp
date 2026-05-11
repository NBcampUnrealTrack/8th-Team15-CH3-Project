// StatusComponent.cpp


#include "Combat/StatusComponent.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();
	HP = MaxHP;
	Stamina = MaxStamina;
	bIsDead = false;
}

void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	StaminaRegenDelay += DeltaTime;

	if (Stamina < MaxStamina)
	{
		if (StaminaRegenDelay > StaminaRegenDelayTime)
		{
			RegenStamina(DeltaTime);
			OnStaminaChanged.Broadcast(Stamina);
		}
	}
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

void UStatusComponent::DrainStamina(float Amount, float DeltaTime)
{
	Stamina = FMath::Max(Stamina - Amount * DeltaTime, 0.0f);
	OnStaminaChanged.Broadcast(Stamina);
	ResetRegenDelay();
}

void UStatusComponent::ConsumeStamina(float Amount)
{
	Stamina = FMath::Max(Stamina - Amount, 0.0f);
	OnStaminaChanged.Broadcast(Stamina);
	ResetRegenDelay();
}

void UStatusComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Min(Stamina + RegenStaminaValue * DeltaTime, MaxStamina);
}

void UStatusComponent::ResetRegenDelay()
{
	StaminaRegenDelay = 0.0f;
}

// getter
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

// setter
void UStatusComponent::SetHP(float NewHP)
{
	HP = NewHP;
	OnHPChanged.Broadcast(HP);
	return;
}

void UStatusComponent::SetATK(float NewATK)
{
	ATK = NewATK;
}

float UStatusComponent::GetHPRatio() const
{
	return MaxHP > 0.f ? HP / MaxHP : 0.f;
}