#include "ItemBase.h"
#include "LJW/GameUtilHeader/GameUtil.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LJW/Character/PrototypeXCharacter.h"
#include "LJW/Item/ItemDatatable.h"
AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneComponent = GameUtil::CreateRootComponet<USceneComponent>(this);
	StaticMeshComponent = GameUtil::CreateComponent<UStaticMeshComponent>(this);
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


	Destroy();
}

FName AItemBase::GetItemID() const
{
	return ItemID;
}

void AItemBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	// Set Item Using DataTable;
	if (!ItemDataTable) return;

	FString ContextString(TEXT("Not Found Item"));
	FItemDatatable* FoundRow = ItemDataTable->FindRow<FItemDatatable>(ItemID, ContextString);

	if (FoundRow)
	{
		// 찾아온 알맹이 데이터 적용
		StaticMeshComponent->SetStaticMesh(FoundRow->ItemMesh.LoadSynchronous());
	}

}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (ItemDataTable)
	{
		FString ContextString(TEXT("BeginPlay Read"));
		FItemDatatable* FoundRow = ItemDataTable->FindRow<FItemDatatable>(ItemID, ContextString);

		if (FoundRow)
		{
			bMagneticOnOff = FoundRow->MagneticOnOff;
		}
	}
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
	FVector NowLocation = GetActorLocation();
	FVector TargetLocation = TargetPlayer->GetActorLocation();
	//FVector TargetDir = (TargetLocation - NowLocation).GetSafeNormal();
	//SetActorLocation(NowLocation + TargetDir * CurrentMageticSpeed * DeltaSeconds);
	CurrentMageticSpeed += (5.f * DeltaSeconds);
	SetActorLocation(FMath::VInterpTo(
		NowLocation,
		TargetLocation,
		DeltaSeconds,
		CurrentMageticSpeed
	));
}

