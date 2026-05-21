// MainGameMode.cpp


#include "MainGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/StatusComponent.h"
#include "LJW/Character/PrototypeXCharacter.h"
#include "LJW/Item/ActorBagComponent.h"
#include "MainGameInstance.h"
void AMainGameMode::BeginPlay()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!PlayerPawn)
	{
		return;
	}

	StatusComp = PlayerPawn->FindComponentByClass<UStatusComponent>();

	if (!StatusComp)
	{
		return;
	}

	StatusComp->OnDeath.AddDynamic(this, &AMainGameMode::SetbIsGameOver);
}

void AMainGameMode::SetbIsGameOver()
{
	bIsGameOver = true;
	OnGameOver.Broadcast();
}

