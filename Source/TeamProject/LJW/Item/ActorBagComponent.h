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
	UFUNCTION()
	void SetActorBag(TArray<FInventorySlot> SetBagWhenGameStart);
	UFUNCTION(BlueprintCallable)
	void AddItemintoBag(FName itemid, int32 itemcount = 1);
	// ====================== getitemingame ======================

	// ====================== useiteminUI ========================
	UFUNCTION(BlueprintCallable)
	void UseItemIndexOnUI(int32 ArrayIndex);
	// ====================== useiteminUI ========================

	// ====================== moveitemslot ========================
	UFUNCTION(BlueprintCallable)
	void MoveItemSlot(int32 FromIndex, int32 ToIndex);
	// ====================== moveitemslot ========================

	// ====================== QuickSlot =======================
	UFUNCTION(BlueprintCallable)
	void SetQuickSlotItemIndexOnUI(int32 QuickSlotIndex);
	UPROPERTY()
	int32 QuickSlotItemIndex = -1;
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
	int32 MaxBagSlot = 54; // 임시로 게임인스턴스에 54
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	TSubclassOf<AActor> ItemBaseClass;
};
