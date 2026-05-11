// StatusComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChangedSignature, float, NewHP);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJECT_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HPBarWidgetClass;

	UPROPERTY()
	UWidgetComponent* HPBarWidgetComp;

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
	void ReceiveDamage(float AttackerATK);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal();

	// Getter 
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHP() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHP() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetATK() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHPRatio() const;

	// setter
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHP(float NewHP);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetATK(float NewATK);
};
