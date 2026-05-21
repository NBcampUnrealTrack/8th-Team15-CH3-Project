// MainGameInstance.cpp


#include "MainGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/StatusComponent.h"
#include "LJW/Item/ActorBagComponent.h"
void UMainGameInstance::SavePlayerStatus(AActor* Player)
{
	if (!Player)
	{
		return;
	}

	UStatusComponent* PlayerStatusComp = Player->GetComponentByClass<UStatusComponent>();

	if (!PlayerStatusComp)
	{
		return;
	}

	PlayerCurrentHP = PlayerStatusComp->GetHP();
	PlayerCurrentATK = PlayerStatusComp->GetATK();
}

void UMainGameInstance::SetPlayerStatus(AActor* Player)
{
	if (!Player)
	{
		return;
	}

	UStatusComponent* PlayerStatusComp = Player->GetComponentByClass<UStatusComponent>();

	if (!PlayerStatusComp)
	{
		return;
	}

	PlayerStatusComp->SetHP(PlayerCurrentHP);
	PlayerStatusComp->SetATK(PlayerCurrentATK);
}

void UMainGameInstance::SetPlayerInventory(TArray<FInventorySlot> Inventory)
{
	PlayerInventory = Inventory;
}

TArray<FInventorySlot> UMainGameInstance::GetPlayerInventory()
{
	return PlayerInventory;
}
