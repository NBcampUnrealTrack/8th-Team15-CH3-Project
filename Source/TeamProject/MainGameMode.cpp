// MainGameMode.cpp


#include "MainGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BluePrint/UserWidget.h"
#include "Combat/StatusComponent.h"

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!PlayerPawn)
	{
		return;
	}

	UStatusComponent* BattleComp = PlayerPawn->FindComponentByClass<UStatusComponent>();

	if (!BattleComp)
	{
		return;
	}

	BattleComp->OnDeath.AddDynamic(this, &AMainGameMode::SetGameOver);
}

void AMainGameMode::SetGameOver()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true); 
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PC)
	{
		return;
	}
		
	UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);

	if (!GameOverWidget)
	{
		return;
	}

	GameOverWidget->AddToViewport();
		
	
}
