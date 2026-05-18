#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

class TEAMPROJECT_API IItemInterface
{
	GENERATED_BODY()
public:

	UFUNCTION()
	virtual void OnMagneticSphereOverlap(
	 UPrimitiveComponent* OverlappedComponent,
	 AActor* OtherActor,
	 UPrimitiveComponent* OtherComp,
	 int32 OtherBodyIndex,
	 bool bFromSweep,
	 const FHitResult& SweepResult) = 0;

	UFUNCTION()
	virtual void OnActivateSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) = 0;

	UFUNCTION()
	virtual void ActivateItem(AActor* Activator) = 0; // put into inventory
	UFUNCTION()
	virtual void UseItem(AActor* Activator) {}

	UFUNCTION()
	virtual FName GetItemID() const = 0;
	UFUNCTION()
	virtual void SetItemID(FName NewItemID) = 0;
};
