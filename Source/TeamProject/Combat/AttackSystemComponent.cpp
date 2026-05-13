// AttackSystemComponent.cpp


#include "Combat/AttackSystemComponent.h"
#include "Combat/StatusComponent.h"
#include "PrototypeXMob.h"
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