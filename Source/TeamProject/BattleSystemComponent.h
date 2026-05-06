//BattleSystemComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJECT_API UBattleSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBattleSystemComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float HP;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxHP = 100;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float ATK = 10;
	UPROPERTY()
	bool bisDead;

public:
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnDeathSignature OnDeath;
	UFUNCTION(BlueprintCallable)
	void DamageCalculation(float attackerATK);
	UFUNCTION(BlueprintCallable)
	void Heal();
};
