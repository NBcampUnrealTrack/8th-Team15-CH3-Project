// AttackSystemComponent.cpp


#include "Combat/AttackSystemComponent.h"
#include "Combat/StatusComponent.h"
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

	if (Cast<APrototypeXCharacter>(GetOwner()))
	{
		if (bUseHitStop)
		{
			HitStop(TargetActor);
		}
		if (bUseHitSlow)
		{
			HitSlow(TargetActor);
		}
	}
}

void UAttackSystemComponent::CheckParry()
{
	TArray<AActor*> Mob; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototypeXMob::StaticClass(), Mob);

	if (Mob.IsEmpty())
	{
		return;
	}

	AActor* ClosestMob = FindClosestActor(Mob);

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

	if (FVector::Dist(ClosestMob->GetActorLocation(), Owner->GetActorLocation()) <= ParryRange)
	{
		if (MobAttackSystemComp->bIsParryWindowOpen)
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
}

void UAttackSystemComponent::PerformHitTrace(FVector point1, FVector point2)
{
	if (!bIsTracing)
	{
		return;
	}

	UStaticMeshComponent* OwnerStaticMeshComp = GetOwner()->GetComponentByClass<UStaticMeshComponent>();

	if (!OwnerStaticMeshComp)
	{
		return;
	}

	FVector CurrentPoint = point1;
	FVector TargetPoint = point2;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CurrentPoint, 
		TargetPoint, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true);

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

	WeaponFirstPoint = CurrentPoint;
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

	if (Cast<APrototypeXCharacter>(Owner))
	{
		Owner->CustomTimeDilation = 0.0f;

		if (!TargetActor)
		{
			return;
		}

		TargetActor->CustomTimeDilation = 0.0f;

		GetWorld()->GetTimerManager().SetTimer(
			Timer,
			[this, Owner, TargetActor]()
			{
				if (!IsValid(Owner))
				{
					return;
				}

				Owner->CustomTimeDilation = 1.0f;

				if (!IsValid(TargetActor))
				{
					return;
				}

				TargetActor->CustomTimeDilation = 1.0f;
			},
			HitStopDelayTime,
			false);
	}
}

void UAttackSystemComponent::HitSlow(AActor* TargetActor)
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return;
	}

	if (Cast<APrototypeXCharacter>(Owner))
	{
		Owner->CustomTimeDilation = HitSlowPlayerTime;

		if (!TargetActor)
		{
			return;
		}

		TargetActor->CustomTimeDilation = HitSlowMobTime;

		FTimerHandle& Handle = HitSlowTimers.FindOrAdd(TargetActor);

		GetWorld()->GetTimerManager().ClearTimer(Handle);

		TWeakObjectPtr<UAttackSystemComponent> WeakThis(this);

		GetWorld()->GetTimerManager().SetTimer(
			Handle,
			[WeakThis, Owner, TargetActor]()
			{
				if (!WeakThis.IsValid())
				{
					return;
				}

				UE_LOG(LogTemp, Warning, TEXT("Call!"))

				if (!IsValid(Owner))
				{
					return;
				}

				Owner->CustomTimeDilation = 1.0f;

				if (!IsValid(TargetActor))
				{
					UE_LOG(LogTemp, Warning, TEXT("TargetActor Not Valid"))
					return;
				}

				TargetActor->CustomTimeDilation = 1.0f;
			},
			HitSlowDelayTime,
			false);
	}
}


void UAttackSystemComponent::SetbIsParryWindowOpen(bool bOpen)
{
	bIsParryWindowOpen = bOpen;
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