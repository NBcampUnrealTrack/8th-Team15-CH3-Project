#include "ItemBase.h"
#include "LJW/GameUtilHeader/GameUtil.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "LJW/Character/PrototypeXCharacter.h"
#include "LJW/Item/ItemDatatable.h"
#include "LJW/Item/ActorBagComponent.h"
#include "MainGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneComponent = GameUtil::CreateRootComponet<USceneComponent>(this);
	StaticMeshComponent = GameUtil::CreateComponent<UStaticMeshComponent>(this);
	NiagaraComponent = GameUtil::CreateComponent<UNiagaraComponent>(this);
	Magnetic_SphereComponent = GameUtil::CreateComponent<USphereComponent>(this);
	Magnetic_SphereComponent->SetSphereRadius(200.f);
	Activate_SphereComponent = GameUtil::CreateComponent<USphereComponent>(this);
	Activate_SphereComponent->SetSphereRadius(40.f);

	Magnetic_SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnMagneticSphereOverlap);
	Activate_SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnActivateSphereOverlap);
}

void AItemBase::OnMagneticSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APrototypeXCharacter* Player = Cast<APrototypeXCharacter>(OtherActor);
	if (Player)
	{
		if (bMagneticOnOff)
		{
			UpVector = SpawnLocation + ((SpawnLocation - Player->GetActorLocation()).GetSafeNormal() * 100.f);
			SetActorTickEnabled(true);
		}
		TargetPlayer = Player;
	}
}

void AItemBase::OnActivateSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APrototypeXCharacter* Player = Cast<APrototypeXCharacter>(OtherActor);
	if (Player)
	{
		if (bMagneticOnOff)
		{
			SetActorTickEnabled(false);
		}
		ActivateItem(OtherActor);
	}
}

void AItemBase::ActivateItem(AActor* Activator)
{
	// ~~ add inventory
	if (UActorBagComponent* ActivatorInventory = Activator->FindComponentByClass<UActorBagComponent>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Activate Item: %s"), *this->GetName());
		ActivatorInventory->AddItemintoBag(ItemID);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActiveSound, GetActorLocation());
	}
	Destroy();
}

FName AItemBase::GetItemID() const
{
	return ItemID;
}

void AItemBase::SetItemID(FName NewItemID)
{
	ItemID = NewItemID;
}

void AItemBase::UpdateItemAppearance()
{
	if (UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetGameInstance()))
	{
		if (UDataTable* ItemDataTables = GameInstance->ItemDataTable)
		{
			FString ContextString(TEXT("BeginPlay Read"));
			FItemDatatable* FoundRow = ItemDataTables->FindRow<FItemDatatable>(ItemID, ContextString);
			if (FoundRow)
			{

				//UE_LOG(LogTemp, Warning, TEXT("DatatableValid"));
				//UStaticMesh* NewMesh = FoundRow->ItemMesh.LoadSynchronous();
				//if (NewMesh)
				//{
				//	UE_LOG(LogTemp, Warning, TEXT("GET MESH"));
				//	StaticMeshComponent->SetStaticMesh(NewMesh);
				//}

				UNiagaraSystem* NewNiagara = FoundRow->ItemEffect.LoadSynchronous();
				if (NewNiagara)
				{
					UE_LOG(LogTemp, Warning, TEXT("GET NIAGARA"));
					NiagaraComponent->SetAsset(NewNiagara);
					NiagaraComponent->Activate(true);
				}

				bMagneticOnOff = FoundRow->MagneticOnOff;

			}
		}
	}
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	SpawnLocation = GetActorLocation();
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ItemToPlayerVinterP(DeltaTime);
	
}

void AItemBase::ItemToPlayerVinterP(float DeltaSeconds)
{
	if (!TargetPlayer || !bMagneticOnOff)
	{
		SetActorTickEnabled(false);
		return;
	}

	if (TakeOneVinterp)
	{
		FVector NowLocation = GetActorLocation();
		FVector TargetLocation = TargetPlayer->GetActorLocation();

		ReverseCurrentMageticSpeed += (1.f * DeltaSeconds);

		SetActorLocation(FMath::VInterpTo(
			NowLocation,
			UpVector,
			DeltaSeconds,
			CurrentMageticSpeed
		));

		if (NowLocation.Z >= (UpVector.Z - 20.f))
		{
			TakeOneVinterp = false;
			TakeTwoVinterp = true;
		}
	}
	if (TakeTwoVinterp)
	{
		FVector NowLocation = GetActorLocation();
		FVector TargetLocation = TargetPlayer->GetActorLocation();
		CurrentMageticSpeed += (1.f * DeltaSeconds);
		SetActorLocation(FMath::VInterpTo(
			NowLocation,
			TargetLocation,
			DeltaSeconds,
			CurrentMageticSpeed
		));
	}
}

