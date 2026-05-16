// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LJW/Item/ItemBase.h"
#include "WeaponItemBase.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API AWeaponItemBase : public AItemBase
{
	GENERATED_BODY()
public:
	AWeaponItemBase();

	virtual void UseItem(AActor* Activator) override;
};
