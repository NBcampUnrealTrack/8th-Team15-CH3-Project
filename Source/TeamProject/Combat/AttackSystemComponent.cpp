// AttackSystemComponent.cpp


#include "Combat/AttackSystemComponent.h"
#include "Combat/StatusComponent.h"
#include "Combat/DataTable/AttackStatRow.h"
#include "PrototypeXMob.h"
#include "LJW/Character/PrototypeXCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

UAttackSystemComponent::UAttackSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttackSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentWeapon = Cast<UStaticMeshComponent>(GetWeapon());

	InitializeFromDataTable();
}

void UAttackSystemComponent::PerformHitTrace(FVector point1, FVector point2)
{
	if (!bIsTracing)
	{
		return;
	}

	FVector CurrentPoint = point1;
	FVector TargetPoint = point2;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CurrentPoint,
		TargetPoint, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false,
		IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true);

	if (bHit)
	{
		if (HitActors.Contains(HitResult.GetActor()))
		{
			return;
		}

		HitActors.Add(HitResult.GetActor());

		CameraShake();
		ApplyDamage(HitResult.GetActor());
	}
}

void UAttackSystemComponent::BeginAttackTrace()
{
	HitActors.Empty();
	bIsTracing = true;
}

void UAttackSystemComponent::EndAttackTrace()
{
	bIsTracing = false;
}

void UAttackSystemComponent::ApplyDamage(AActor* TargetActor, float ParryDamageMultipiler)
{
	if (!TargetActor)
	{
		return;
	}

	UStatusComponent* TargetStatusComp = TargetActor->FindComponentByClass<UStatusComponent>();

	if (!TargetStatusComp)
	{
		return;
	}

	UStatusComponent* AttackerStatusComp = GetAttackerStatusComponent();

	if (!AttackerStatusComp)
	{
		return;
	}

	TargetStatusComp->ReceiveDamage(AttackerStatusComp->GetATK() * ParryDamageMultipiler);


	if (bUseHitStop)
	{
		HitStop(TargetActor);
	}
	if (bUseHitSlow)
	{
		HitSlow(TargetActor);
	}

}

TWeakObjectPtr<UActorComponent> UAttackSystemComponent::GetWeapon()
{
	TArray<UActorComponent*> Weapon;

	TArray<UActorComponent*> MyWeapon = GetOwner()->GetComponentsByTag(UStaticMeshComponent::StaticClass(), WeaponTag);

	if (MyWeapon.Num())
	{
		TWeakObjectPtr<UActorComponent> TempWeapon = MyWeapon[0];

		return TempWeapon;
	}

	return nullptr;
}

UStatusComponent* UAttackSystemComponent::GetAttackerStatusComponent()
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return nullptr;
	}

	if (!OwnerStatusComp)
	{
		OwnerStatusComp = Owner->GetComponentByClass<UStatusComponent>();
	}

	return OwnerStatusComp;
}

void UAttackSystemComponent::HitStop(AActor* TargetActor)
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return;
	}

	TWeakObjectPtr<AActor> WeakOwner(Owner);

	if (!TargetActor)
	{
		return;
	}

	TWeakObjectPtr<AActor> WeakTargetActor(TargetActor);

	if (!WeakOwner.IsValid())
	{
		return;
	}

	WeakOwner->CustomTimeDilation = 0.0f;

	if (!WeakTargetActor.IsValid())
	{
		return;
	}

	WeakTargetActor->CustomTimeDilation = 0.0f;

	TWeakObjectPtr<UAttackSystemComponent> WeakThis(this);

	if (!WeakThis.IsValid())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		Timer,
		[WeakThis, WeakOwner, WeakTargetActor]()
		{
			if (!WeakOwner.IsValid())
			{
				return;
			}

			WeakOwner->CustomTimeDilation = 1.0f;

			if (!WeakTargetActor.IsValid())
			{
				return;
			}

			WeakTargetActor->CustomTimeDilation = 1.0f;
		},
		HitStopDelayTime,
		false);
}

void UAttackSystemComponent::HitSlow(AActor* TargetActor)
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return;
	}

	TWeakObjectPtr<AActor> WeakOwner(Owner);

	if (!WeakOwner.IsValid())
	{
		return;
	}

	WeakOwner->CustomTimeDilation = HitSlowPlayerTime;

	if (!TargetActor)
	{
		return;
	}

	TWeakObjectPtr<AActor> WeakTargetActor(TargetActor);
	
	if (!WeakTargetActor.IsValid())
	{
		return;
	}

	WeakTargetActor->CustomTimeDilation = HitSlowMobTime;

	FTimerHandle& Handle = HitSlowTimers.FindOrAdd(WeakTargetActor.Get());

	GetWorld()->GetTimerManager().ClearTimer(Handle);

	TWeakObjectPtr<UAttackSystemComponent> WeakThis(this);

	if (!WeakThis.IsValid())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		Handle,
		[WeakThis, WeakOwner, WeakTargetActor]()
		{
			if (!WeakOwner.IsValid())
			{
				return;
			}

			WeakOwner->CustomTimeDilation = 1.0f;

			if (!WeakTargetActor.IsValid())
			{
				return;
			}

			WeakTargetActor->CustomTimeDilation = 1.0f;
			WeakThis.Get()->HitSlowTimers.Remove(WeakTargetActor.Get()); 
			
		},
		HitSlowDelayTime,
		false);
}

void UAttackSystemComponent::CheckParry()
{
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> Mob;

	TArray<FHitResult> Mobs;

	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(),
		ParryRange, TraceTypeQuery2, false, IgnoreActors, EDrawDebugTrace::ForDuration, Mobs, true, FLinearColor::Blue);

	for (int i = 0; i < Mobs.Num(); ++i)
	{
		Mob.Add(Mobs[i].GetActor());
	}

	if (Mob.IsEmpty())
	{
		return;
	}

	TArray<AActor*> FilteredMob;

	for (int i = 0; i < Mob.Num(); ++i)
	{
		if (FVector::DotProduct(GetOwner()->GetActorForwardVector(), 
			(Mob[i]->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal()) > ParryDotThreshold)
		{
			FilteredMob.Add(Mob[i]);
		}
	}

	if (FilteredMob.IsEmpty())
	{
		return;
	}

	AActor* ClosestMob = FindClosestActor(FilteredMob);

	if (!ClosestMob)
	{
		return;
	}

	UAttackSystemComponent* MobAttackSystemComp = ClosestMob->GetComponentByClass<UAttackSystemComponent>();

	if (!MobAttackSystemComp)
	{
		return;
	}

	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return;
	}

	if (MobAttackSystemComp->GetbIsParryWindowOpen())
	{
		OnParrySuccess.Broadcast();
		ApplyDamage(ClosestMob, ParryDamageMultiplier);
		UE_LOG(LogTemp, Warning, TEXT("Your Parry Success!"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Your Parry Failed!"))
		return;
	}

}

AActor* UAttackSystemComponent::FindClosestActor(TArray<AActor*> Actors)
{
	if (Actors.IsEmpty())
	{
		return nullptr;
	}

	AActor* FindActor = Actors[0];
	float FindRange = FVector::Dist(Actors[0]->GetActorLocation(), GetOwner()->GetActorLocation());

	for (int i = 1; i < Actors.Num(); ++i)
	{
		if (FindRange > FVector::Dist(Actors[i]->GetActorLocation(), GetOwner()->GetActorLocation()))
		{
			FindRange = FVector::Dist(Actors[i]->GetActorLocation(), GetOwner()->GetActorLocation());
			FindActor = Actors[i];
		}

	}

	return FindActor;
}

// Getter
bool UAttackSystemComponent::GetbIsParryWindowOpen()
{
	return bIsParryWindowOpen;
}

// Setter
void UAttackSystemComponent::SetbIsParryWindowOpen(bool bOpen)
{
	bIsParryWindowOpen = bOpen;
}

void UAttackSystemComponent::InitializeFromDataTable()
{
	if (!StatTable)
	{
		return;
	}

	FAttackStatRow* Row = StatTable->FindRow<FAttackStatRow>(RowName, TEXT("Not Find RowName"));

	if (!Row)
	{
		return;
	}

	ParryRange = Row->ParryRange;
	ParryDamageMultiplier = Row->ParryDamageMultiplier;
	HitStopDelayTime = Row->HitStopDelayTime;
	HitSlowDelayTime = Row->HitSlowDelayTime;
	HitSlowPlayerTime = Row->HitSlowPlayerTime;
	HitSlowMobTime = Row->HitSlowMobTime;
	ParryDotThreshold = Row->ParryDotThreshold;
}

