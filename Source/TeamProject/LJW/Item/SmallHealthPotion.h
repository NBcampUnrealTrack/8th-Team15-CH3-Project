#pragma once

#include "CoreMinimal.h"
#include "LJW/Item/HealthPotionItemBase.h"
#include "SmallHealthPotion.generated.h"

UCLASS()
class TEAMPROJECT_API ASmallHealthPotion: public AHealthPotionItemBase
{
	GENERATED_BODY()
public:
	ASmallHealthPotion();

	virtual void UseItem(AActor* Activator) override;

};
