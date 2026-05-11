// AttackSystemComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackSystemComponent.generated.h"

class UStatusComponent;
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
	UStatusComponent* OwnerStatusComp;

public:	
	UFUNCTION(BlueprintCallable)
	void ApplyDamage(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	UStatusComponent* GetAttackerStatusComponent();
};
