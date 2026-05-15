// AttackSystemComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnParrySuccessSignature);

class UStatusComponent;
class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
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

	// Hit Slow
	UPROPERTY(EditDefaultsOnly, Category = "HitStop")
	float HitSlowDelayTime = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "HitStop")
	float HitSlowPlayerTime = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "HitStop")
	float HitSlowMobTime = 0.3f;

private:
	UPROPERTY()
	TMap<AActor*, FTimerHandle> HitSlowTimers;

public:
	// Parry
	UPROPERTY(BlueprintAssignable, Category = "Parry")
	FOnParrySuccessSignature OnParrySuccess;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	bool bIsParryWindowOpen;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float ParryRange = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float ParryDamageMultiplier = 3.0f;

	// Hit
	UPROPERTY()
	FVector WeaponFirstPoint;

	UPROPERTY()
	TSet<AActor*> HitActors;

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

	UFUNCTION(BlueprintImplementableEvent)
	void CameraShake();

private:
	void HitStop(AActor* TargetActor);

	void HitSlow(AActor* TargetActor);

	void ApplyDamage(AActor* TargetActor, float ParryDamageMultipiler = 1.0f);

	UStatusComponent* GetAttackerStatusComponent();

	AActor* FindClosestActor(TArray<AActor*> Actors);

public:
	// Setter
	UFUNCTION(BlueprintCallable, Category = "Parry")
	void SetbIsParryWindowOpen(bool bOpen);
};
