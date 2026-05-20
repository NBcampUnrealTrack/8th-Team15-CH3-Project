// StatusComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature); // GameMode
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHalfHealthReached, AActor*, Boss); // Boss
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, AActor*, Character); // Character
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChangedSignature, float, NewHP); // UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedSignature, float, NewStamina); // UI

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

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float DEF = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float Absorption = 0.1f;

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

	UPROPERTY()
	bool bIsInvincible = false;

// DeleGate
public:
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHPChangedSignature OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHalfHealthReached OnOnHalfHealthReached;

	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaChangedSignature OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

private:
	bool bHalfHealthDelegateFired = false;

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

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetATK() const;

	UFUNCTION(BlueprintPure, Category = "Statmina")
	float GetStamina() const;

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintPure, Category = "Invincibility")
	bool GetbIsInvincible() const;

	// setter
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetbIsDead(bool NewbIsDead);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHP(float NewHP);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHP(float NewMaxHP);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetATK(float NewATK);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetStamina(float NewStamina);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetMaxStamina(float NewMaxStamina);

	UFUNCTION(BlueprintCallable, Category = "Invincibility")
	void SetbIsInvincible(bool NewbInvincible);
};
