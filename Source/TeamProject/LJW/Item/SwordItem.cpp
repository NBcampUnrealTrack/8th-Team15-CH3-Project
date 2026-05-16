#include "LJW/Item/SwordItem.h"

ASwordItem::ASwordItem()
{
	ItemID = "NormalSword";
	Amount = 20.f;
}

void ASwordItem::UseItem(AActor* Activator)
{
	Super::UseItem(Activator);
}
