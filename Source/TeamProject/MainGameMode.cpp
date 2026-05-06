// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BluePrint/UserWidget.h"
#include "HealthComponent.h"

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		UHealthComponent* BattleComp = PlayerPawn->FindComponentByClass<UHealthComponent>();
		if (BattleComp)
		{
			BattleComp->OnDeath.AddDynamic(this, &AMainGameMode::SetGameOver);
		}
	}
}

void AMainGameMode::SetGameOver()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true); 
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PC != nullptr)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);

		if (GameOverWidget != nullptr)
		{
			GameOverWidget->AddToViewport();
		}
	}
}
