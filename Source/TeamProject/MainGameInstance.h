// MainGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

class UDataTable;
struct FInventorySlot;
enum class EItemType : uint8;

UCLASS()
class TEAMPROJECT_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<UDataTable> ItemDataTable;
private:
	UPROPERTY()
	float PlayerCurrentHP;

	UPROPERTY()
	float PlayerCurrentATK;

	UPROPERTY()
	TArray<FInventorySlot> PlayerInventory;

	UPROPERTY()
	TMap<EItemType, FName> PlayerWeaponBag;

public:
	UFUNCTION()
	void SavePlayerStatus(AActor* Player);

	UFUNCTION()
	void SetPlayerStatus(AActor* Player);

	UFUNCTION()
	void SetPlayerInventory(TArray<FInventorySlot> Inventory);
	UFUNCTION()
	TArray<FInventorySlot> GetPlayerInventory();

	UFUNCTION()
	void SetPlayerWeaponBag(TMap<EItemType, FName> Inventory);
	UFUNCTION()
	TMap<EItemType, FName> GetPlayerWeaponBag();

	virtual void Init() override;
};
