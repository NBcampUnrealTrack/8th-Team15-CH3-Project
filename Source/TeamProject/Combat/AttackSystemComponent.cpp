// AttackSystemComponent.cpp


#include "Combat/AttackSystemComponent.h"
#include "Combat/HealthComponent.h"

UAttackSystemComponent::UAttackSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAttackSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttackSystemComponent::SetAttackerWeaponColision()
{
	if  (WeaponColision != nullptr)
	{
		WeaponColision->OnComponentBeginOverlap.AddDynamic(this, &UAttackSystemComponent::OnOverlapAttack);
	}
}

void UAttackSystemComponent::OnOverlapAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	UHealthComponent* TargetHealthComp = OtherActor->FindComponentByClass<UHealthComponent>();

	if (TargetHealthComp != nullptr)
	{
		UHealthComponent* AttackerHealthComp = GetAttackerHealthComponent();
		if (AttackerHealthComp != nullptr)
		{
			TargetHealthComp->TakeDamage(AttackerHealthComp->GetATK());
		}
	}
}

void UAttackSystemComponent::ApplyDamage(AActor* TargetActor)
{
}



UHealthComponent* UAttackSystemComponent::GetAttackerHealthComponent()
{
	if (!OwnerHealthComp)
	{
		OwnerHealthComp = GetOwner()->GetComponentByClass<UHealthComponent>();
	}

	return OwnerHealthComp;
}


