#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorBagComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWhenBagChangedSignal);

class AActor;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJECT_API UActorBagComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActorBagComponent();
	// need to write > itemspawn on death, add item
	UFUNCTION(BlueprintCallable)
	void DropItemOnDeath();

	// ====================== getitemingame ======================
	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlot> GetActorBag() const;
	UFUNCTION(BlueprintCallable)
	void AddItemintoBag(FName itemid, int32 itemcount = 1);
	// ====================== getitemingame ======================

	// ====================== useiteminUI ========================
	UFUNCTION(BlueprintCallable)
	void UseItemIndexOnUI(int32 ArrayIndex);
	// ====================== useiteminUI ========================

	// ====================== QuickSlot =======================
	UFUNCTION(BlueprintCallable)
	void SetQuickSlotItemIndexOnUI(int32 QuickSlotIndex);
	UFUNCTION()
	int32 QuickSlotItemIndex;
	UFUNCTION(BlueprintCallable)
	int32 GetQuickSlotItemIndex();
	UFUNCTION(BlueprintCallable)
	int32 GetMaxBagSlot() const;
	UFUNCTION(BlueprintCallable)
	int32 GetItemCount() const;
	// ====================== DELEGATE WHEN CHANGED ITEMS ========================
	UPROPERTY(BlueprintAssignable, Category = "DELEGATE")
	FOnWhenBagChangedSignal OnBagChanged;
	// ====================== DELEGATE WHEN CHANGED ITEMS ========================
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	TArray<FInventorySlot> ActorBag;
	UPROPERTY()
	int32 MaxBagSlot = 15;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	TSubclassOf<AActor> ItemBaseClass;
};
