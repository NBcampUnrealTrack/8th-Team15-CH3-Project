#pragma once

#include "CoreMinimal.h"
#include "LJW/Item/WeaponItemBase.h"
#include "SwordItem.generated.h"

UCLASS()
class TEAMPROJECT_API ASwordItem : public AWeaponItemBase
{
	GENERATED_BODY()
public:
	ASwordItem();

	virtual void UseItem(AActor* Activator) override;
};
