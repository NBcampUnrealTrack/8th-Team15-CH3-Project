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

	HP = FMath::Max(HP - AttackerATK, 0.0f);
	UE_LOG(LogTemp, Warning, TEXT("HP: %f"), HP);
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
