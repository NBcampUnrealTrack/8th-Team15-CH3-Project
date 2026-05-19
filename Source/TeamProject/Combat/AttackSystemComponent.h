// AttackSystemComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "AttackSystemComponent.generated.h"

class UStatusComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnParrySuccessSignature);

UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class TEAMPROJECT_API UAttackSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttackSystemComponent();

protected:
	virtual void BeginPlay() override;

// Hit
public:
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

private:
	UPROPERTY()
	UStatusComponent* OwnerStatusComp;

public:
	UFUNCTION(BlueprintCallable)
	void PerformHitTrace(FVector point1, FVector point2);

	UFUNCTION(BlueprintCallable)
	void BeginAttackTrace();

	UFUNCTION(BlueprintCallable)
	void EndAttackTrace();

	UFUNCTION(BlueprintImplementableEvent)
	void CameraShake();

private:
	void ApplyDamage(AActor* TargetActor, float ParryDamageMultipiler = 1.0f);

	UFUNCTION()
	TWeakObjectPtr<UActorComponent> GetWeapon();

	UStatusComponent* GetAttackerStatusComponent();

// Hit Stop
public:
	UPROPERTY(EditDefaultsOnly, Category = "HitStop")
	bool bUseHitStop = false;

	UPROPERTY(EditDefaultsOnly, Category = "HitStop")
	float HitStopDelayTime = 0.2f;

private:
	UFUNCTION()
	void HitStop(AActor* TargetActor);

// Hit Slow
public:
	UPROPERTY(EditDefaultsOnly, Category = "HitSlow")
	bool bUseHitSlow = false;

	UPROPERTY(EditDefaultsOnly, Category = "HitSlow")
	float HitSlowDelayTime = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "HitSlow")
	float HitSlowPlayerTime = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "HitSlow")
	float HitSlowMobTime = 0.3f;

private:
	FTimerHandle Timer;

	UPROPERTY()
	TMap<AActor*, FTimerHandle> HitSlowTimers;

private:
	void HitSlow(AActor* TargetActor);

// Parry
public:
	UPROPERTY(BlueprintAssignable, Category = "Parry")
	FOnParrySuccessSignature OnParrySuccess;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float ParryRange = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float ParryDamageMultiplier = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float ParryDotThreshold = 0.7f;

private:
	bool bIsParryWindowOpen;

public:
	UFUNCTION(BlueprintCallable)
	void CheckParry();

	AActor* FindClosestActor(TArray<AActor*> Actors);

public:
	UFUNCTION(BlueprintCallable, Category = "Parry")
	bool GetbIsParryWindowOpen();

	UFUNCTION(BlueprintCallable, Category = "Parry")
	void SetbIsParryWindowOpen(bool bOpen);

// Boss Pattern
public:
	UPROPERTY(EditDefaultsOnly, Category = "BossRadialAttack")
	float BossRadialAttackFirstRadius = 36.0f;

	UPROPERTY(EditDefaultsOnly, Category = "BossRadialAttack")
	float BossRadialAttackMaxRadius = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "BossRadialAttack")
	float BossRadialAttackDamageMutiplier = 1.5f;

public:
	UFUNCTION(BlueprintCallable, Category = "BossPattern")
	void PerformRadialAttack(float Radius);

// DataTable
public:
	UPROPERTY(EditDefaultsOnly, Category = "DataTable")
	UDataTable* StatTable;

	UPROPERTY(EditDefaultsOnly, Category = "DataTable")
	FName RowName;

private:
	void InitializeFromDataTable();
};
