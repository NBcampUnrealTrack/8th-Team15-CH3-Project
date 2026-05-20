// AttackSystemComponent.cpp


#include "Combat/AttackSystemComponent.h"
#include "Combat/StatusComponent.h"
#include "Combat/DataTable/AttackStatRow.h"
#include "Combat/DataTable/BossAttackStatRow.h"
#include "PrototypeXMob.h"
#include "LJW/Character/PrototypeXCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Hit
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

void UAttackSystemComponent::ApplyDamage(AActor* TargetActor, float ParryDamageMultiplier)
{
	if (!TargetActor)
	{
		return;
	}

	UStatusComponent* TargetStatusComp = TargetActor->FindComponentByClass<UStatusComponent>();

	if (!IsValid(TargetStatusComp))
	{
		return;
	}

	UStatusComponent* AttackerStatusComp = GetAttackerStatusComponent();

	if (!IsValid(AttackerStatusComp))
	{
		return;
	}

	if (!TargetStatusComp->GetbIsInvincible())
	{
		TargetStatusComp->ReceiveDamage(AttackerStatusComp->GetATK() * ParryDamageMultiplier);
	}

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

	if (!IsValid(OwnerStatusComp))
	{
		OwnerStatusComp = Owner->GetComponentByClass<UStatusComponent>();
	}

	return OwnerStatusComp;
}

//Hit Stop
void UAttackSystemComponent::HitStop(AActor* TargetActor)
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}

	if (!IsValid(TargetActor))
	{
		return;
	}

	Owner->CustomTimeDilation = 0.0f;

	TargetActor->CustomTimeDilation = 0.0f;

	TWeakObjectPtr<AActor> WeakOwner(Owner);
	TWeakObjectPtr<AActor> WeakTargetActor(TargetActor);

	GetWorld()->GetTimerManager().SetTimer(
		Timer,
		[WeakOwner, WeakTargetActor]()
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

// Hit Slow
void UAttackSystemComponent::HitSlow(AActor* TargetActor)
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}

	Owner->CustomTimeDilation = HitSlowPlayerTime;

	if (!IsValid(TargetActor))
	{
		return;
	}

	TargetActor->CustomTimeDilation = HitSlowMobTime;

	FTimerHandle& Handle = HitSlowTimers.FindOrAdd(TargetActor);

	GetWorld()->GetTimerManager().ClearTimer(Handle);

	TWeakObjectPtr<UAttackSystemComponent> WeakThis(this);
	TWeakObjectPtr<AActor> WeakOwner(Owner);
	TWeakObjectPtr<AActor> WeakTargetActor(TargetActor);

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

// Parry
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

	AActor* ClosestMob = FindClosestActor(Mob);

	if (!IsValid(ClosestMob))
	{
		return;
	}

	if (FVector::DotProduct(GetOwner()->GetActorForwardVector(),
		(ClosestMob->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal()) <= ParryDotThreshold)
	{
		return;
	}

	UAttackSystemComponent* MobAttackSystemComp = ClosestMob->GetComponentByClass<UAttackSystemComponent>();

	if (!IsValid(MobAttackSystemComp))
	{
		return;
	}

	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}

	if (MobAttackSystemComp->GetbIsParryWindowOpen())
	{
		OnParrySuccess.Broadcast(ClosestMob);
		ApplyDamage(ClosestMob, ParryDamageMultiplier);
		UE_LOG(LogTemp, Warning, TEXT("Your Parry Success!"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Your Parry Failed!"))
		return;
	}
}

AActor* UAttackSystemComponent::FindClosestActor(const TArray<AActor*> &Actors)
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

bool UAttackSystemComponent::GetbIsParryWindowOpen()
{
	return bIsParryWindowOpen;
}

void UAttackSystemComponent::SetbIsParryWindowOpen(bool bOpen)
{
	bIsParryWindowOpen = bOpen;
}

// Boss Pattern
void UAttackSystemComponent::PerformRadialAttack(float Radius)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(GetOwner());

	TArray<AActor*> OutActors;

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetOwner()->GetActorLocation(), Radius,
		ObjectTypes, nullptr, ActorToIgnore, OutActors);

	DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), Radius, 12, FColor::Red, false, 0.1f);

	for (int i = 0; i < OutActors.Num(); ++i)
	{
		if (!DamagedActors.Contains(OutActors[i]))
		{
			ApplyDamage(OutActors[i], BossRadialAttackDamageMultiplier);
			DamagedActors.Add(OutActors[i]);
		}
	}
}

// DataTable
void UAttackSystemComponent::InitializeFromDataTable()
{
	if (StatTable)
	{
		FAttackStatRow* AttackStatRow = StatTable->FindRow<FAttackStatRow>(RowName, TEXT("Not Find RowName"));
		if (AttackStatRow)
		{
			ParryRange = AttackStatRow->ParryRange;
			ParryDamageMultiplier = AttackStatRow->ParryDamageMultiplier;
			HitStopDelayTime = AttackStatRow->HitStopDelayTime;
			HitSlowDelayTime = AttackStatRow->HitSlowDelayTime;
			HitSlowPlayerTime = AttackStatRow->HitSlowPlayerTime;
			HitSlowMobTime = AttackStatRow->HitSlowMobTime;
			ParryDotThreshold = AttackStatRow->ParryDotThreshold;
		}
	}

	if (BossAttackStatTable)
	{
		FBossAttackStatRow* BossAttackStatRow = BossAttackStatTable->FindRow<FBossAttackStatRow>(BossRowName, TEXT("Not Find RowName"));
		if (BossAttackStatRow)
		{
			BossRadialAttackFirstRadius = BossAttackStatRow->BossRadialAttackFirstRadius;
			BossRadialAttackMaxRadius = BossAttackStatRow->BossRadialAttackMaxRadius;
			BossRadialAttackExpandSpeed = BossAttackStatRow->BossRadialAttackExpandSpeed;
			BossRadialAttackDamageMultiplier = BossAttackStatRow->BossRadialAttackDamageMultiplier;
		}
	}
}

