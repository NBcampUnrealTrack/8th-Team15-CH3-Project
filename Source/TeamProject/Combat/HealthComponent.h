// HealthComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChangedSignature, float, NewHP);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAMPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float HP;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHP = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ATK = 10.0f; // TODO 추후 StatusComponent로 분리

	UPROPERTY()
	bool bIsDead = false;

public:
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHPChangedSignature OnHPChanged;

public:
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float AttackerATK);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal();

	// Getter 
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE bool IsDead() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetHP() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHP() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetATK() const;
};