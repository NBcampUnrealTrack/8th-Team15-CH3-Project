#include "LJW/Item/ActorBagComponent.h"
#include "LJW/Item/ItemDatatable.h"
#include "MainGameInstance.h"
#include "Combat/StatusComponent.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "LJW/Item/ItemBase.h"
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

void UActorBagComponent::AddItemintoBag(FName itemid, int32 itemcount)
{
	if (MaxBagSlot > ActorBag.Num())
	{
		for (FInventorySlot& Slots : ActorBag)
		{
			if (Slots.ItemID == itemid)
			{
				UE_LOG(LogTemp, Warning, TEXT("Add Count"));
				Slots.Count += itemcount;
				return;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Add Item"));
		FInventorySlot Item({ itemid, itemcount });
		ActorBag.Add(Item);
		return;
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("FULL INVENTORY BAG")));
		}
	}
	OnBagChanged.Broadcast();
}

void UActorBagComponent::UseItemIndexOnUI(int32 ArrayIndex)
{
	if (!ActorBag.IsValidIndex(ArrayIndex)) //index check
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("ItemCount is %d... OverLoad Slot"), ActorBag.Num()));
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
				EItemType ItemType = FoundRow->ItemType;

				switch (ItemType)
				{
				case EItemType::POTION:
					// heal
				{
					StatusCompoent->SetHP(StatusCompoent->GetHP() + FoundRow->Amount);
					ActorBag[ArrayIndex].Count--;

					if (ActorBag[ArrayIndex].Count <= 0)
					{
						ActorBag.RemoveAt(ArrayIndex);
					}

					break;
				}
				case EItemType::EQUIPMENT:
					// set socket mesh
					StatusCompoent->SetATK(StatusCompoent->GetATK() + FoundRow->Amount);
					ActorBag[ArrayIndex].Count--;

					if (ActorBag[ArrayIndex].Count <= 0)
					{
						ActorBag.RemoveAt(ArrayIndex);
					}
					break;
				default:
					break;
				}
			}
			OnBagChanged.Broadcast();
		}
	}
}

int32 UActorBagComponent::GetMaxBagSlot() const
{
	return MaxBagSlot;
}

int32 UActorBagComponent::GetItemCount() const
{
	return ActorBag.Num();
}

void UActorBagComponent::BeginPlay()
{
	Super::BeginPlay();
}
