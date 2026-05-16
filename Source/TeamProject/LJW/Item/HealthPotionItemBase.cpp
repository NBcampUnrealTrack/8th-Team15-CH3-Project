#include "LJW/Item/HealthPotionItemBase.h"
#include "Combat/StatusComponent.h"

AHealthPotionItemBase::AHealthPotionItemBase()
{

}

void AHealthPotionItemBase::UseItem(AActor* Activator)
{
	UStatusComponent* StatusComponent = Activator->FindComponentByClass<UStatusComponent>();
	//StatusComponent에서 현재체력 + Amount
	if (StatusComponent)
	{
		StatusComponent->SetHP(StatusComponent->GetHP() + Amount);
	}
}
