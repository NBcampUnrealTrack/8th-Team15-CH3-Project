// MainGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

class UDataTable;
struct FInventorySlot;

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
public:
	UFUNCTION()
	void SavePlayerStatus(AActor* Player);

	UFUNCTION()
	void SetPlayerStatus(AActor* Player);

	UFUNCTION()
	void SetPlayerInventory(TArray<FInventorySlot> Inventory);
	UFUNCTION()
	TArray<FInventorySlot> GetPlayerInventory();
};
