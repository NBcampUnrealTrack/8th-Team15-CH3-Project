#pragma once

#include "CoreMinimal.h"
#include "LJW/Item/ItemBase.h"
#include "HealthPotionItemBase.generated.h"

UCLASS()
class TEAMPROJECT_API AHealthPotionItemBase : public AItemBase
{
	GENERATED_BODY()
public:
	AHealthPotionItemBase();

	virtual void UseItem(AActor* Activator) override;
};
