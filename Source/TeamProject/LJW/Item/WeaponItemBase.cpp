#include "LJW/Item/WeaponItemBase.h"
#include "Combat/StatusComponent.h"

AWeaponItemBase::AWeaponItemBase()
{

}

void AWeaponItemBase::UseItem(AActor* Activator)
{
	UStatusComponent* StatusComponent = Activator->FindComponentByClass<UStatusComponent>();
	//StatusComponent에서 현재체력 + Amount
	if (StatusComponent)
	{
		StatusComponent->SetATK(StatusComponent->GetATK() + Amount);
	}

}
