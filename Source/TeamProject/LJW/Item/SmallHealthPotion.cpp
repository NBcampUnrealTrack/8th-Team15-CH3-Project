#include "LJW/Item/SmallHealthPotion.h"
ASmallHealthPotion::ASmallHealthPotion()
{
	ItemID = "SmallHealthPotion";
	Amount = 30.f;
}

void ASmallHealthPotion::UseItem(AActor* Activator)
{
	Super::UseItem(Activator);
}
