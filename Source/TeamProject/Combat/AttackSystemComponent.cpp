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
	ApplyDamage(OtherActor);
}

void UAttackSystemComponent::ApplyDamage(AActor* TargetActor)
{
	if (TargetActor == nullptr)
	{
		return;
	}

	UHealthComponent* TargetHealthComp = TargetActor->FindComponentByClass<UHealthComponent>();

	if (TargetHealthComp != nullptr)
	{
		UHealthComponent* AttackerHealthComp = GetAttackerHealthComponent();
		if (AttackerHealthComp != nullptr)
		{
			TargetHealthComp->TakeDamage(AttackerHealthComp->GetATK());
		}
	}
}



UHealthComponent* UAttackSystemComponent::GetAttackerHealthComponent()
{
	if (!OwnerHealthComp)
	{
		OwnerHealthComp = GetOwner()->GetComponentByClass<UHealthComponent>();
	}

	return OwnerHealthComp;
}


