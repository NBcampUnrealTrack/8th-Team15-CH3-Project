#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LJW/Item/ItemInterface.h"
#include "ItemBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;

UCLASS(ABSTRACT)
class TEAMPROJECT_API AItemBase
	: public AActor
	, public IItemInterface
{
	GENERATED_BODY()
	
public:	
	AItemBase();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> Magnetic_SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> Activate_SphereComponent;

	virtual void OnMagneticSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void OnActivateSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void ActivateItem(AActor* Activator) override;

	virtual void Tick(float DeltaSeconds) override;
	void ItemToPlayerVinterP(float DeltaSeconds);
	AActor* TargetPlayer = nullptr;
	float CurrentMageticSpeed = 2.f;

	virtual FName GetItemID() const override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Setting")
	TObjectPtr<UDataTable> ItemDataTable;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amount;

	bool bMagneticOnOff = false;
};
