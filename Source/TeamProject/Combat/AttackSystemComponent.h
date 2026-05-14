// AttackSystemComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnParrySuccessSignature);

class UStatusComponent;
class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJECT_API UAttackSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttackSystemComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UStatusComponent* OwnerStatusComp;

	FTimerHandle Timer;

public:
	// Hit Stop
	UPROPERTY(EditDefaultsOnly, Category = "HitStop")
	float HitStopDelayTime = 0.2f;

	// Parry
	UPROPERTY(BlueprintAssignable, Category = "Parry")
	FOnParrySuccessSignature OnParrySuccess;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	bool bIsParryWindowOpen;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float ParryRange = 150.0f;

	// Hit
	UPROPERTY()
	FVector WeaponFirstPoint;

	UPROPERTY()
	TSet<AActor*> HitActors;

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float HitTraceSphereRadius = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Hit")
	bool bIsTracing = false;

	UPROPERTY(EditAnywhere, Category = "Hit")
	FName WeaponTag = FName(TEXT("Weapon"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit")
	TWeakObjectPtr<UStaticMeshComponent> CurrentWeapon;

public:
	UFUNCTION(BlueprintCallable)
	void CheckParry();

	UFUNCTION(BlueprintCallable)
	void PerformHitTrace(FVector point1, FVector point2);

	UFUNCTION(BlueprintCallable)
	void BeginAttackTrace();

	UFUNCTION(BlueprintCallable)
	void EndAttackTrace();

	UFUNCTION()
	TWeakObjectPtr<UActorComponent> GetWeapon();


private:
	void HitStop(AActor* TargetActor);

	void ApplyDamage(AActor* TargetActor);

	UStatusComponent* GetAttackerStatusComponent();

public:
	// Setter
	UFUNCTION(BlueprintCallable, Category = "Parry")
	void SetbIsParryWindowOpen(bool bOpen);
};
