// HealthComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

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
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ATK = 10.0f;

	UPROPERTY()
	bool bIsDead = false;

public:
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal();

	// Getter 함수들
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
};