// StatusComponent.cpp


#include "Combat/StatusComponent.h"
#include "Combat/DataTable/CharacterStatRow.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeFromDataTable();

	HP = MaxHP;
	OnHPChanged.Broadcast(HP);
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

	float TrueDamage = FMath::Max((AttackerATK - DEF) * (1 - Absorption), AttackerATK * 0.1f);
	SetHP(HP - TrueDamage);

	if (GetHP() <= 0.0f)
	{
		bIsDead = true;
		OnDeath.Broadcast();
	}
}

void UStatusComponent::DrainStamina(float Amount, float DeltaTime)
{
	SetStamina(Stamina - Amount * DeltaTime);
	
	ResetRegenDelay();
}

void UStatusComponent::ConsumeStamina(float Amount)
{
	SetStamina(Stamina - Amount);
	
	ResetRegenDelay();
}

void UStatusComponent::RegenStamina(float DeltaTime)
{
	SetStamina(Stamina + RegenStaminaValue * DeltaTime);
}

void UStatusComponent::ResetRegenDelay()
{
	StaminaRegenDelay = 0.0f;
}

void UStatusComponent::InitializeFromDataTable()
{
	if (!StatTable)
	{
		return;
	}

	FCharacterStatRow* Row = StatTable->FindRow<FCharacterStatRow>(RowName, TEXT("Not Find RowName"));

	if (!Row)
	{
		return;
	}

	MaxHP = Row->MaxHP;
	MaxStamina = Row->MaxStamina;
	RegenStaminaValue = Row->RegenStaminaValue;
	StaminaRegenDelayTime = Row->StaminaRegenDelayTime;
	ATK = Row->ATK;
	DEF = Row->DEF;
	Absorption = Row->Absorption;
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

float UStatusComponent::GetStamina() const
{
	return Stamina;
}

float UStatusComponent::GetMaxStamina() const
{
	return MaxStamina;
}

// setter
void UStatusComponent::SetbIsDead(bool NewbIsDead)
{
	bIsDead = NewbIsDead;
}

void UStatusComponent::SetHP(float NewHP)
{
	HP = FMath::Clamp(NewHP, 0.0f, MaxHP);
	OnHPChanged.Broadcast(HP);
}

void UStatusComponent::SetMaxHP(float NewMaxHP)
{
	MaxHP = NewMaxHP;
}

void UStatusComponent::SetATK(float NewATK)
{
	ATK = NewATK;
}

void UStatusComponent::SetStamina(float NewStamina)
{
	Stamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(Stamina);
}

void UStatusComponent::SetMaxStamina(float NewMaxStamina)
{
	MaxStamina = NewMaxStamina;
}

