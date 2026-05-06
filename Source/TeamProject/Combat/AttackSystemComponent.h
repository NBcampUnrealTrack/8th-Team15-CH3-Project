// AttackSystemComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackSystemComponent.generated.h"

class UHealthComponent;
class UPrimitiveComponent;

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
	UHealthComponent* OwnerHealthComp;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponColision")
	UPrimitiveComponent* WeaponColision;

public:	
	UFUNCTION(BlueprintCallable)
	void SetAttackerWeaponColision();

	UFUNCTION(BlueprintCallable)
	void OnOverlapAttack(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	UHealthComponent* GetAttackerHealthComponent();
};
