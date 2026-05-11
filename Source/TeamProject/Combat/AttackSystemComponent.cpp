// AttackSystemComponent.cpp


#include "Combat/AttackSystemComponent.h"
#include "Combat/StatusComponent.h"

UAttackSystemComponent::UAttackSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAttackSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

//void UAttackSystemComponent::SetAttackerWeaponColision()
//{
//	if  (WeaponColision != nullptr)
//	{
//		WeaponColision->OnComponentBeginOverlap.AddDynamic(this, &UAttackSystemComponent::OnOverlapAttack);
//	}
//}

void UAttackSystemComponent::OnOverlapAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ApplyDamage(OtherActor);
}

void UAttackSystemComponent::ApplyDamage(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}

	UStatusComponent* TargetStatusComp = TargetActor->FindComponentByClass<UStatusComponent>();

	if (!TargetStatusComp)
	{
		return;
	}

	UStatusComponent* AttackerStatusComp = GetAttackerStatusComponent();

	if (!AttackerStatusComp)
	{
		return;
	}

	TargetStatusComp->ReceiveDamage(AttackerStatusComp->GetATK());
}



UStatusComponent* UAttackSystemComponent::GetAttackerStatusComponent()
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return nullptr;
	}

	if (!OwnerStatusComp)
	{
		OwnerStatusComp = Owner->GetComponentByClass<UStatusComponent>();
	}

	return OwnerStatusComp;
}


