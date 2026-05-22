#include "LJW/Item/ActorBagComponent.h"
#include "LJW/Item/ItemDatatable.h"
#include "MainGameInstance.h"
#include "Combat/StatusComponent.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "LJW/Item/ItemBase.h"
#include "LJW/Character/PrototypeXCharacter.h"

UActorBagComponent::UActorBagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorBagComponent::DropItemOnDeath()
{
	FVector OriginSpawnLocation = FVector::ZeroVector;

	if (ACharacter* Player = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* CharacterMesh = Player->GetMesh())
		{
			FBoxSphereBounds MeshBound = CharacterMesh->GetLocalBounds();
			float MeshHeight = MeshBound.BoxExtent.Z;
			FVector FloorLocation = Player->GetActorLocation();
			FloorLocation.Z += MeshHeight;
			OriginSpawnLocation = FloorLocation;
		}
	}
	else if (AActor* Actor = Cast<AActor>(GetOwner()))
	{
		if (UStaticMeshComponent* ActorMesh = Actor->FindComponentByClass<UStaticMeshComponent>())
		{
			FVector LocalMin;
			FVector LocalMax;

			ActorMesh->GetLocalBounds(LocalMin, LocalMax);
			FVector FloorLocation = Actor->GetActorLocation();
			FloorLocation.Z += LocalMax.Z;
			OriginSpawnLocation = FloorLocation;
		}
	}

	if (UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		FString DebugString(TEXT("Cant Find Item On Bag When Spawning"));
		for (FInventorySlot& Slots : ActorBag)
		{
			if (Slots.ItemID == NAME_None || Slots.Count <= 0)
			{
				continue;
			}

			if (FItemDatatable* FoundRow = GameInstance->ItemDataTable->FindRow<FItemDatatable>(Slots.ItemID, DebugString))
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //Spawn even in wall

				for (int i = 0; i < Slots.Count; i++)
				{
					FVector SpawnLocation = OriginSpawnLocation;
					float RandomLocationX = FMath::FRandRange(-25.f, 25.f);
					float RandomLocationY = FMath::FRandRange(-25.f, 25.f);
					SpawnLocation.X += RandomLocationX;
					SpawnLocation.Y += RandomLocationY;

					AActor* SpawnedItem = GetWorld()->SpawnActor<AActor>(
						ItemBaseClass,
						SpawnLocation,
						FRotator::ZeroRotator,
						SpawnParams
					);

					if (SpawnedItem)
					{
						UE_LOG(LogTemp, Warning, TEXT("Spawn Completed : %s"), *Slots.ItemID.ToString());

						if (AItemBase* ItemBase = Cast<AItemBase>(SpawnedItem))
						{
							ItemBase->SetItemID(Slots.ItemID);
							ItemBase->UpdateItemAppearance();
						}
					}
				}
			}
			else
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("NO MATCHING ITEM ID WHEN SPAWNING")));
				}
			}
		}
	}
}

TArray<FInventorySlot> UActorBagComponent::GetActorBag() const
{
	return ActorBag;
}

void UActorBagComponent::SetActorBag(TArray<FInventorySlot> SetBagWhenGameStart)
{
	ActorBag = SetBagWhenGameStart;
	OnBagChanged.Broadcast();
}

void UActorBagComponent::AddItemintoBag(FName itemid, int32 itemcount)
{
	//while (ActorBag.Num() < MaxBagSlot)
	//{
	//	ActorBag.Add(FInventorySlot());
	//}

	for (FInventorySlot& Slot : ActorBag)
	{
		if (Slot.ItemID == itemid)
		{
			Slot.Count += itemcount;
			OnBagChanged.Broadcast();
			return;
		}
	}

	for (FInventorySlot& Slot : ActorBag)
	{
		if (Slot.ItemID == NAME_None)
		{
			Slot.ItemID = itemid;
			Slot.Count = itemcount;
			OnBagChanged.Broadcast();
			return;
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("FULL INVENTORY BAG"));
	}
}

void UActorBagComponent::UseItemIndexOnUI(int32 ArrayIndex)
{
	if (!ActorBag.IsValidIndex(ArrayIndex)) // maxslot넘어가는 index일때
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Invalid Item Index")));
		}
		return;
	}

	if (ActorBag[ArrayIndex].ItemID == FName(TEXT("NAME_None")))//index check
	{

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("This Slot Is NAME_NONE Item")));
		}
		return;
	}

	if (ActorBag[ArrayIndex].Count <= 0) // count 0이하일때
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("This Slot is ZERO COUNT")));
		}
		return;
	}

	if (UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		FString DebugString(TEXT("Cant Find Item On Bag"));
		if (FItemDatatable* FoundRow = GameInstance->ItemDataTable->FindRow<FItemDatatable>(ActorBag[ArrayIndex].ItemID, DebugString))
		{
			if (UStatusComponent* StatusCompoent = GetOwner()->FindComponentByClass<UStatusComponent>())
			{
				if (APrototypeXCharacter* Player = Cast<APrototypeXCharacter>(GetOwner()))
				{
					if (Player->ItemUse_MontagePlay(ActorBag[ArrayIndex].ItemID)) // MontagePlayFunction
					{
						// (IsRollingMontagePlaying || bIsAttacking || bIsOnJumpping || IsItemUsing) CANNOT USE ITEM
						EItemType SetItemType = FoundRow->ItemType;

						switch (SetItemType)
						{
						case EItemType::POTION:
							Player->IsPlayerDrinking = true;
							StatusCompoent->SetHP(StatusCompoent->GetHP() + FoundRow->Amount);
							break;
						case EItemType::EQUIPMENT:

							StatusCompoent->SetATK(FoundRow->Amount);
							break;
						}

						ActorBag[ArrayIndex].Count--;

						if (ActorBag[ArrayIndex].Count <= 0)
						{
							ActorBag[ArrayIndex] = FInventorySlot();

							//ActorBag[ArrayIndex] = FInventorySlot();
							//if (QuickSlotItemIndex == ArrayIndex)
							//{
							//	QuickSlotItemIndex = -1;
							//}
							//else if (QuickSlotItemIndex > ArrayIndex)
							//{
							//	QuickSlotItemIndex--;
							//}
							//ActorBag.RemoveAt(ArrayIndex);
						}
						OnBagChanged.Broadcast();
					}
					else
					{
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("YOU CANNOT USE ITEM WHEN MOVING")));
						}
					}
				}

			}
		}
	}
}

void UActorBagComponent::MoveItemSlot(int32 FromIndex, int32 ToIndex)
{
	if (FromIndex == ToIndex) return;
	if (FromIndex < 0 || ToIndex < 0) return;
	if (FromIndex >= MaxBagSlot || ToIndex >= MaxBagSlot) return;

	//while (ActorBag.Num() < MaxBagSlot)
	//{
	//	ActorBag.Add(FInventorySlot());
	//}

	if (ActorBag[FromIndex].ItemID == NAME_None) return;

	FInventorySlot Temp = ActorBag[ToIndex];
	ActorBag[ToIndex] = ActorBag[FromIndex];
	ActorBag[FromIndex] = Temp;

	if (QuickSlotItemIndex == FromIndex) // 양방향 퀵슬롯 체크
	{
		QuickSlotItemIndex = ToIndex;
	}
	else if (QuickSlotItemIndex == ToIndex)
	{
		QuickSlotItemIndex = FromIndex;
	}

	OnBagChanged.Broadcast();

}

// Have to Position Last Logic On UI
void UActorBagComponent::SetQuickSlotItemIndexOnUI(int32 QuickSlotIndex)
{
	QuickSlotItemIndex = QuickSlotIndex;
	OnBagChanged.Broadcast();
}

int32 UActorBagComponent::GetQuickSlotItemIndex()
{
	return QuickSlotItemIndex;
}

int32 UActorBagComponent::GetMaxBagSlot() const
{
	return MaxBagSlot;
}

int32 UActorBagComponent::GetItemCount() const
{
	int32 ItemCountInBag = 0;
	for (const FInventorySlot& Slot : ActorBag)
	{
		if (Slot.ItemID != NAME_None)
		{
			ItemCountInBag++;
		}
	}
	return ItemCountInBag;
}

void UActorBagComponent::BeginPlay()
{
	Super::BeginPlay();
	ActorBag.Init(FInventorySlot(), MaxBagSlot);
}
