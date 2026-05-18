// StatusComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChangedSignature, float, NewHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedSignature, float, NewStamina);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAMPROJECT_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatusComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
// Status
private:
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float HP;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHP = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ATK = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float Stamina;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float RegenStaminaValue = 20.0f;

	float StaminaRegenDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaRegenDelayTime = 1.0f;

	UPROPERTY()
	bool bIsDead = false;

public:
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHPChangedSignature OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaChangedSignature OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ReceiveDamage(float AttackerATK);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void DrainStamina(float Amount, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void RegenStamina(float DeltaTime);

private:
	void ResetRegenDelay();

// DataTable
public:
	UPROPERTY(EditDefaultsOnly, Category = "DataTable")
	UDataTable* StatTable;

	UPROPERTY(EditDefaultsOnly, Category = "DataTable")
	FName RowName;

private:
	void InitializeFromDataTable();

public:
	// Getter 
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHP() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHP() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetATK() const;

	UFUNCTION(BlueprintCallable, Category = "Statmina")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxStamina() const;

	// setter
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHP(float NewHP);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetATK(float NewATK);
};
