// AttackSystemComponent.cpp


#include "Combat/AttackSystemComponent.h"
#include "Combat/StatusComponent.h"
#include "PrototypeXMob.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

UAttackSystemComponent::UAttackSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttackSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttackSystemComponent::ApplyDamage(AActor* TargetActor)
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

	TargetStatusComp->ReceiveDamage(AttackerStatusComp->GetATK());
	HitStop(TargetActor);
}

void UAttackSystemComponent::CheckParry()
{
	TArray<AActor*> Mob; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototypeXMob::StaticClass(), Mob);

	if (Mob.IsEmpty())
	{
		return;
	}

	UAttackSystemComponent* MobAttackSystemComp = Mob[0]->GetComponentByClass<UAttackSystemComponent>();

	if (!MobAttackSystemComp)
	{
		return;
	}
	
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return;
	}

	if (FVector::Dist(Mob[0]->GetActorLocation(), Owner->GetActorLocation()) <= ParryRange)
	{
		if (MobAttackSystemComp->bIsParryWindowOpen)
		{
			OnParrySuccess.Broadcast();
			UE_LOG(LogTemp, Warning, TEXT("Your Parry Success!"))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Your Parry Failed!"))
				return;
		}
	}
}

void UAttackSystemComponent::PerformHitTrace(FName SocketName)
{
	if (!bIsTracing)
	{
		return;
	}

	USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();

	if (!OwnerSkeletalMeshComp)
	{
		return;
	}

	FVector CurrentPoint = OwnerSkeletalMeshComp->GetSocketLocation(SocketName);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), WeaponFirstPoint, CurrentPoint, 
		HitTraceSphereRadius, TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true);

	if (bHit)
	{
		if (HitActors.Contains(HitResult.GetActor()))
		{
			return;
		}
		
		HitActors.Add(HitResult.GetActor());

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

void UAttackSystemComponent::SetbIsParryWindowOpen(bool bOpen)
{
	bIsParryWindowOpen = bOpen;
}