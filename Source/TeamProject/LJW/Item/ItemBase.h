#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "LJW/Item/ItemInterface.h"
#include "ItemBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class USoundBase;

UCLASS()
class TEAMPROJECT_API AItemBase
	: public AActor
	, public IItemInterface
{
	GENERATED_BODY()
	
public:	
	AItemBase();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Magnetic_SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
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

	bool TakeTwoVinterp = false;
	float CurrentMageticSpeed = 2.f;

	bool TakeOneVinterp = true;
	float ReverseCurrentMageticSpeed = 4.f;

	virtual FName GetItemID() const override;
	virtual void SetItemID(FName NewItemID) override;

	void UpdateItemAppearance();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	bool bMagneticOnOff = false;
	// ============================================

	FVector SpawnLocation;
	FVector UpVector;
};
