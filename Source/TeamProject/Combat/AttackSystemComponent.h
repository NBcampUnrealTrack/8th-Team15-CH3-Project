// AttackSystemComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackSystemComponent.generated.h"

class UStatusComponent;

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
	UPROPERTY(EditDefaultsOnly, Category = "HitStop")
	float HitStopDelayTime = 0.05f;

private:
	float PreAttackStart;
	float PreAttackEnd;
	float PlayerParryInput;

public:
	UFUNCTION(BlueprintCallable)
	void ApplyDamage(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void CheckParry();

private:
	void HitStop(AActor* TargetActor);

	UStatusComponent* GetAttackerStatusComponent();

public:
	// Parry Setter
	UFUNCTION(BlueprintCallable)
	void SetPreAttackStartTime(float NewTime);

	UFUNCTION(BlueprintCallable)
	void SetPreAttackEndTime(float NewTime);

	UFUNCTION(BlueprintCallable)
	void SetPlayerParryInputTime(float NewTime);
};
